#include "raft_consensus.hpp"

#include <grpcpp/grpcpp.h>

#include <chrono>
#include <cstdlib>

#include "abeille.grpc.pb.h"
#include "core.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

RaftConsensus::RaftConsensus(Core *core) noexcept
    : id_(core->config_.GetId()), core_(core), num_peers_threads_(&core_->num_peers_threads_) {}

RaftConsensus::~RaftConsensus() {
  if (timer_thread_.joinable()) timer_thread_.join();
}

void RaftConsensus::Run() {
  timer_thread_ = std::thread(&RaftConsensus::timerThreadMain, this);
  LOG_INFO("RaftConsensus was started");
}

void RaftConsensus::stepDown(uint64_t term) {}

// If election timeout elapses wihtout recieving AppendEntries
// initiates startNewElection()
void RaftConsensus::timerThreadMain() {
  std::unique_lock<std::mutex> lock_guard(core_->mutex);
  while (!exiting_) {
    if (start_new_election_at_ >= election_timeout_ && voted_for_ == 0) {
      // Resetting all votes ?
      LOG_INFO("New election was started");
      startNewElection();
    }
    core_->state_changed_.wait_until(lock_guard, start_new_election_at_);
  }
}

void RaftConsensus::resetElectionTimer() {
  int64_t rand_duration =
      rand() % election_timeout_.time_since_epoch().count() + election_timeout_.time_since_epoch().count();
  std::chrono::milliseconds duration(rand_duration);
  start_new_election_at_ = Clock::now() + duration;
  core_->state_changed_.notify_all();
}

void RaftConsensus::becomeLeader() {
  LOG_INFO("I'm the leader now (term %lu)", current_term_);
  state_ = State::LEADER;
  leader_id_ = id_;
  start_new_election_at_ = TimePoint::max();
}

void RaftConsensus::startNewElection() {
  if (leader_id_ > 0) {
    LOG_INFO("Starting election in term %lu (haven't heard from leader %lu lately", current_term_ + 1, leader_id_);
  } else if (state_ == State::CANDIDATE) {
    LOG_INFO("Starting election in term %lu (previous term %lu timed out)", current_term_ + 1, current_term_);
  } else {
    LOG_INFO("Starting election in term %lu", current_term_ + 1);
  }

  ++current_term_;
  voted_for_ = id_;
  resetElectionTimer();
  core_->raft_pool_->BeginRequestVote();

  // if we are the only server
  if (core_->raft_pool_->QuorumAll()) becomeLeader();
}

void RaftConsensus::appendEntry(Peer &peer) {
  AppendEntryRequest request;
  request.set_term(current_term_);
  request.set_leader_id(id_);
  request.set_prev_log_index(peer.next_index_ - 1);
  request.set_prev_log_term(log_->GetEntry(peer.next_index_ - 1)->term());
  request.set_leader_commit(commit_index);

  if (log_->LastIndex() >= peer.next_index_) request.set_allocated_entry(log_->GetEntry(peer.next_index_));

  AppendEntryResponse response;
  grpc::ClientContext context;
  grpc::Status status = peer.stub_->AppendEntry(&context, request, &response);

  if (!status.ok()) {
    LOG_INFO("AppendEntry to peer %lu failed", peer.id_);
    return;
  }

  if (current_term_ != request.term() || peer.exiting_) return;

  (response.success()) ? ++peer.next_index_ : --peer.next_index_;
}

void RaftConsensus::requestVote(Peer &peer) {
  RequestVoteRequest request;
  request.set_term(current_term_);
  request.set_candidate_id(id_);
  request.set_last_log_entry(log_->LastIndex());
  request.set_last_log_term(log_->GetEntry(log_->LastIndex())->term());

  RequestVoteResponse response;
  grpc::ClientContext context;
  grpc::Status status = peer.stub_->RequestVote(&context, request, &response);

  if (!status.ok()) {
    LOG_INFO("AppendEntry to peer %lu failed", peer.id_);
    return;
  }

  if (current_term_ != request.term() || state_ != State::CANDIDATE || peer.exiting_) return;

  if (response.term() > current_term_)
    stepDown(response.term());
  else {
    peer.vote_request_done_ = true;
    core_->state_changed_.notify_all();

    if (response.vote_granted()) {
      peer.have_vote_ = true;
      if (core_->raft_pool_->QuorumAll()) becomeLeader();
    } else
      LOG_INFO("RequestEntry was declined");
  }
}

void RaftConsensus::peerThreadMain(std::shared_ptr<Peer> peer) {
  LOG_INFO("Peer thread for server %lu", peer->id_);
  std::unique_lock<std::mutex> lock_guard(core_->mutex);
  TimePoint wait_until = TimePoint::min();

  // issue RPC or sleeps on the cv
  while (!peer->exiting_) {
    switch (state_) {
      case State::FOLLOWER:
        wait_until = TimePoint::max();
        break;

      case State::CANDIDATE:
        if (!peer->vote_request_done_)
          requestVote(*peer);
        else
          wait_until = TimePoint::max();
        break;

      case State::LEADER:
        if (log_->LastIndex() >= peer->next_index_ || peer->next_heartbeat_time_ <= Clock::now())
          appendEntry(*peer);
        else
          wait_until = peer->next_heartbeat_time_;
        break;
    }
  }

  core_->state_changed_.wait_until(lock_guard, wait_until);
}

// TODO: implement:

void RaftConsensus::HandleAppendEntries(const AppendEntryRequest *msg, AppendEntryResponse *resp) {}

void RaftConsensus::HandleRequestVote(const RequestVoteRequest *msg, RequestVoteResponse *resp) {}

void RaftConsensus::Shutdown(){};

}  // namespace raft_node
}  // namespace abeille
