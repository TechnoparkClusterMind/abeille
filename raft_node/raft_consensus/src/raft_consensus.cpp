#include "raft_consensus.hpp"

#include <grpcpp/grpcpp.h>

#include <chrono>
#include <cstdlib>

#include "abeille.grpc.pb.h"
#include "core.hpp"
#include "logger.hpp"
#include "types.hpp"

namespace abeille {
namespace raft_node {

RaftConsensus::RaftConsensus(Core *core) noexcept
    : id_(core->config_.GetId()), core_(core), log_(), state_machine_(log_) {}

RaftConsensus::~RaftConsensus() {
  if (!shutdown_) Shutdown();

  if (timer_thread_.joinable()) {
    timer_thread_.join();
  }
}

void RaftConsensus::AddPeer() noexcept { ++core_->num_peers_thread; }

RaftConsensus::Status RaftConsensus::Run() {
  try {
    timer_thread_ = std::thread(&RaftConsensus::timerThreadMain, this);
  } catch (...) {
    LOG_ERROR("Unable to start RaftConsensus");
    return Status(Status::Code::FAILURE);
  }

  LOG_INFO("RaftConsensus was started");
  return Status(Status::Code::OK);
}

void RaftConsensus::stepDown(uint64_t new_term) {
  if (current_term_ < new_term) {
    LOG_INFO("Stepping down to %lu", new_term);
    current_term_ = new_term;
    leader_id_ = 0;
    voted_for_ = 0;
    state_ = State::FOLLOWER;

  } else if (state_ != State::FOLLOWER) {
    state_ = State::FOLLOWER;
  }

  // was leader
  if (start_new_election_at_ == TimePoint::max())
    resetElectionTimer();
  if (hold_election_for == TimePoint::max())
    hold_election_for = TimePoint::min();
}

// If election timeout elapses without recieving AppendEntries
// initiates startNewElection()
void RaftConsensus::timerThreadMain() {
  std::unique_lock<std::mutex> lock_guard(core_->mutex);
  while (!shutdown_) {
    if (start_new_election_at_ >= election_timeout_ && voted_for_ == 0) {
      LOG_INFO("New election was started");
      startNewElection();
    }
    raft_state_changed_.wait_until(lock_guard, start_new_election_at_,
                                   [this] { return shutdown_; });
  }
}

void RaftConsensus::Shutdown() noexcept {
  shutdown_ = true;
  raft_state_changed_.notify_all();
}

void RaftConsensus::resetElectionTimer() {
  int64_t rand_duration =
      rand() % election_timeout_.time_since_epoch().count() +
      election_timeout_.time_since_epoch().count();
  std::chrono::milliseconds duration(rand_duration);
  start_new_election_at_ = Clock::now() + duration;
  raft_state_changed_.notify_all();
}

void RaftConsensus::becomeLeader() {
  LOG_INFO("I'm the leader now (term %lu)", current_term_);
  state_ = State::LEADER;
  leader_id_ = id_;
  start_new_election_at_ = TimePoint::max();
  hold_election_for = TimePoint::max();
}

void RaftConsensus::startNewElection() {
  if (leader_id_ > 0) {
    LOG_INFO(
        "Starting election in term %lu (haven't heard from leader %lu lately",
        current_term_ + 1, leader_id_);
  } else if (state_ == State::CANDIDATE) {
    LOG_INFO("Starting election in term %lu (previous term %lu timed out)",
             current_term_ + 1, current_term_);
  } else {
    LOG_INFO("Starting election in term %lu", current_term_ + 1);
  }

  ++current_term_;
  voted_for_ = id_;
  resetElectionTimer();
  // Resetting all peers:
  core_->raft_pool_->BeginRequestVote();

  // if we are the only server
  if (core_->raft_pool_->MajorityVotes()) becomeLeader();
}

void RaftConsensus::appendEntry(Peer &peer) {
  AppendEntryRequest request;
  request.set_term(current_term_);
  request.set_leader_id(id_);
  request.set_prev_log_index(peer.next_index_ - 1);
  request.set_prev_log_term(log_->GetEntry(peer.next_index_ - 1)->term());
  request.set_leader_commit(state_machine_->GetCommitIndex());

  if (log_->LastIndex() >= peer.next_index_)
    request.set_allocated_entry(log_->GetEntry(peer.next_index_));

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

  if (current_term_ != request.term() || state_ != State::CANDIDATE ||
      peer.exiting_)
    return;

  if (response.term() > current_term_)
    stepDown(response.term());
  else {
    peer.vote_request_done_ = true;
    raft_state_changed_.notify_all();

    if (response.vote_granted()) {
      peer.have_vote_ = true;
      if (core_->raft_pool_->MajorityVotes()) becomeLeader();
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
        if (log_->LastIndex() >= peer->next_index_ ||
            peer->next_heartbeat_time_ <= Clock::now())
          appendEntry(*peer);
        else
          wait_until = peer->next_heartbeat_time_;
        break;
    }
  }

  core_->pool_state_changed_.wait_until(lock_guard, wait_until);
}

void RaftConsensus::HandleAppendEntry(const AppendEntryRequest *msg,
                                        AppendEntryResponse *resp) {
  LOG_INFO("AppendEntry request was recieved from %lu", resp->leader_id());
  std::lock_guard<std::mutex> lockGuard(mutex_);

  // Set response to rejection at first
  // Will be changed
  resp->set_term(current_term_);
  resp->set_success(false);

  if (msg->term() < current_term_) {
    LOG_INFO("AppendEntry caller is stale");
    return;
  }

  if (msg->term() > current_term_) {
    LOG_INFO("Recieved AppendEntry with %lu term\n",
             "Our term is %lu. Updating term...", msg->term(), current_term_);
    // Our term will be updated in stepDown
    resp->set_term(msg->term());
  }

  // To follower state, update term, reset timer
  stepDown(msg->term());
  resetElectionTimer();
  hold_election_for = Clock::now() + election_timeout_;

  // Record LeaderId
  if (leader_id_ == 0) {
    leader_id_ = msg->leader_id();
    LOG_INFO("All heil new leader %lu", leader_id_);
  }

  if (msg->prev_log_index() > log_->LastIndex()) {
    LOG_INFO("Rejecting AppendEntry... Recieved %lu index. Mine is %lu",
             msg->prev_log_index(), log_->LastIndex());
    return;
  }

  if (msg->prev_log_index != log_->GetEntry(msg->prev_log_index()).term()) {
    LOG_INFO("Rejecting AppendEntry... Terms don't agree");
    return;
  }

  // Applying AppendEntry
  resp->set_success(true);

  if (msg->has_entry()) {
    log_->Purge(msg->prev_log_index());
    log_->Append(msg->entry());
  }

  if (state_machine_->GetCommitIndex < msg->leader_commit()) {
    LOG_INFO("Commiting new entries");
    // Commits all entries to leader's commit idx
    state_machine_->Commit(msg->leader_commit());
  }
}

void RaftConsensus::HandleRequestVote(const RequestVoteRequest *msg,
                                      RequestVoteResponse *resp) {
  LOG_INFO("RequestVote from candidate %lu was recieved", msg->candidate_id());
  std::lock_guard<std::mutex> lockGuard(mutex_);

  bool is_ok = (msg->last_log_term() > log_->LastTerm() ||
                (msg->last_log_term() == log_->LastTerm() &&
                 msg->last_log_index() >= log_->LastIndex()));

  if (hold_election_for > Clock::now()) {
    LOG_INFO("Rejecting RequestVote... Heard from a leader recently\n");
    resp->set_term(current_term_);
    resp->set_vote_granted(false);
    return;
  }

  if (msg->term() > current_term_) {
    LOG_INFO("Recieved RequestVote. Changing term of the current server...");
    stepDown(msg->term());
  }

  if (msg->term() == current_term_ && is_ok && !voted_for_) {
    LOG_INFO("Voting for %lu", msg->candidate_id());
    stepDown(current_term_);
    resetElectionTimer();
    voted_for_ = msg->candidate_id();
  }

  resp->set_term(current_term_);
  resp->set_vote_granted(voted_for_ == msg->candidate_id());
}

}  // namespace raft_node
}  // namespace abeille
