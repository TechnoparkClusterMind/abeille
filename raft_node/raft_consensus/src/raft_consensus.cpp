#include "raft_consensus.hpp"
#include "grpcpp/grpc.h"
#import "abeille.pb.h"
#include "core.hpp"
#include <chrono>
#include <cstdlib>
#include <macroses.hpp>

namespace abeille {
namespace raft_node {

RaftConsensus::RaftConsensus(Core *core)
    : core_(core), state_(State::FOLLOWER), id_(id_), leader_id_(0), log_(),
      clock_(), timer_thread_(), heartbeat_period_(election_timeout_ / 2),
      election_timeout_(std::chrono::milliseconds(500)), current_term_(0),
      voted_for_(0), start_new_election_at_(timePoint::max()), exiting_(false) {
}

RaftConsensus::~RaftConsensus() {
  if (timer_thread_.joinable())
    timer_thread_.join();
}

void RaftConsensus::Run() {
  timer_thread_ = std::thread(&RaftConsensus::timerThreadMain, this);
  LOG("RaftConsensus was started\n");
}

// If election timeout elapses wihtout recieving AppendEntries
// initiates startNewElection()
void RaftConsensus::timerThreadMain() {
  std::unique_lock<std::mutex> lock_guard(core_->mutex);
  while (!exiting_) {
    if (start_new_election_at_ >= election_timeout_ && voted_for_ == 0) {
      // Resetting all votes ?
      LOG("New election was started\n");
      startNewElection();
    }
    core_->state_changed_.wait_until(lock_guard, start_new_election_at_);
  }
}

void RaftConsensus::resetElectionTimer() {
  start_new_election_at_ =
      clock_.now() + election_timeout_ +
      timePoint(std::chrono::milliseconds(
          std::rand() % election_timeout_.time_since_epoch().count()));

  core_->state_changed_.notify_all();
}

void RaftConsensus::becomeLeader() {
  LOG("I'm the leader now (term %lu)\n", current_term_);
  state_ = State::LEADER;
  leader_id_ = id_;
  start_new_election_at_ = timePoint::max();
}

void RaftConsensus::startNewElection() {
  if (leader_id_ > 0)
    LOG("Starting election in term %lu "
        "(haven't heard from leader %lu lately",
        current_term_ + 1, leader_id_);
  else if (state_ == State::CANDIDATE)
    LOG("Starting election in term %lu"
        "(previous term %lu timed out)",
        current_term_ + 1, current_term_);
  else
    LOG("Starting election in term %lu", current_term_ + 1);

  ++current_term_;
  voted_for_ = id_;
  resetElectionTimer();
  core_->raft_pool->BeginRequestVote();

  // if we are the only server
  if (core_->raft_pool->QuorumAll())
      becomeLeader();
}

void RaftConsensus::appendEntry(Peer &peer) {
  AppendEntryRequest request;
  request.set_term(current_term_);
  request.set_leader_id(id_);
  request.set_prev_log_index(peer.next_index_ - 1);
  request.set_prev_log_term(log_->GetEntry(peer.next_index_ - 1).term);
  request.set_leader_commit(commit_index);

  if (log_->LastIndex() >= peer.next_index_)
    request.set_Entry(log_->GetEntry(peer.next_index_));

  AppendEntryResponse response;
  grpc::ClientContext context;
  grpc::Status status = peer.stub_->AppendEntry(&context, request, &response);

  if (!status.ok())
  {
      LOG("AppendEntry to peer %lu failed\n", peer.id_);
      return;
  }

  if (current_term_ != request.term() || peer.exiting_)
      return;

  (response.success()) ? ++peer.next_index_ : --peer.next_index_;
}

void requestVote(Peer &peer) {
    RequestVoteRequest request;
    request.set_term(current_term_);
    request.set_candidate_id(id_);
    request.set_last_log_entry(log_->LastIndex());
    request.set_last_log_term(log_->GetEntry(log_->LastIndex).term);

    RequestVoteResponse response;
    grpc::ClientContext context;
    grpc::Status status = peer.stub_->RequestVote(&context, request, &response);

    if (!status.ok())
    {
        LOG("AppendEntry to peer %lu failed\n", peer.id_);
        return;
    }

    if (current_term_ != request.term() || state_ != State::CANDIDATE ||
        peer.exiting_)
        return;

    if (response.term() > current_term_)
        stepDown(response.term());
    else {
        peer.vote_request_done_ = true;
        core_->state_changed_.notify_all();

        if (response.vote_granted()) {
            peer.have_vote_ = true;
            if (core_->raft_pool->QuorumAll())
                becomeLeader();
        } else
            LOG("RequestEntry was declined\n");
    }
}

void RaftConsensus::peerThreadMain(std::shared_ptr<Peer> peer) {
  LOG("Peer thread for server %lu", peer->id_);
  std::unique_lock<std::mutex> lock_guard(core_->mutex);
  timePoint wait_until = timePoint::min();

  // issue RPC or sleeps on the cv
  while (!peer->exiting) {
    switch (state_) {
    case State::FOLLOWER:
      wait_until = timePoint::max();
      break;

    case State::CANDIDATE:
      if (!peer->vote_request_done)
        requestVote(lock_guard, *peer);
      else
        wait_until = timePoint::max();
      break;

    case State::LEADER:
      if (log_->LastIndex() >= peer->next_index_ ||
          peer->next_heartbeat_time_ <= now)
        appendEntry(lock_guard, *peer);
      else
        wait_until = peer->next_heartbeat_time_;
      break;
    }
  }

  core_->state_changed_.wait_until(lock_guard, wait_until);
}

} // namespace raft_node
} // namespace abeille
