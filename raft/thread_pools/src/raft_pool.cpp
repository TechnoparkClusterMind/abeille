#include "raft/thread_pools/include/raft_pool.hpp"

#include <grpcpp/grpcpp.h>

#include <algorithm>

#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"
#include "utils/include/types.hpp"

namespace abeille {
namespace raft {

RaftPool::RaftPool(RaftPool::RaftRef raft) : raft_(raft) {
  // FIXME: It's not ok. Maybe singleton global Config
  auto peers_adresses = raft_->core_->config_.GetPeers();
  for (const auto& addr : peers_adresses) {
    ServerId temp_id = address2uint(addr);
    peers_.push_back(std::make_shared<Peer>(
        grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()), raft_,
        temp_id));
  }
}

RaftPool::Status RaftPool::Run() {
  for (auto& peer : peers_) {
    peer->Run(peer);
  }
  return Status();
}

void RaftPool::beginRequestVote() {
  for (auto& peer : peers_) peer->BeginRequestVote();
}

bool RaftPool::majorityVotes() {
  uint64_t votes_num = std::count_if(
      peers_.cbegin(), peers_.cend(),
      [](const RaftPool::PeerRef peer) { return peer->HaveVote(); });

  bool vote_yourself = raft_->voted_for_ == raft_->id_;
  return votes_num + vote_yourself > peers_.size() / 2;
}

Index RaftPool::poolCommitIndex(Index& prev_commit_idx) {
  bool stored_majority = true;
  Index temp_index = prev_commit_idx;

  while (stored_majority) {
    ++temp_index;
    stored_majority =
        static_cast<long unsigned int>(std::count_if(
            peers_.cbegin(), peers_.cend(), [&temp_index](const PeerRef peer) {
              return peer->GetMatchIndex() >= temp_index;
            })) > peers_.size() / 2;
  }

  return --temp_index;
}

void RaftPool::AppendAll(const Entry& entry) {
  LOG_TRACE();
  raft_->log_->Append(entry);
  raft_->raft_state_changed_.notify_all();
}

void RaftPool::Shutdown() {
  for (auto& peer : peers_) {
    peer->Shutdown();
  }
}

}  // namespace raft
}  // namespace abeille
