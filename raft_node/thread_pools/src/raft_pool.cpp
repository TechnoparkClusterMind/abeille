#include "raft_pool.hpp"

#include <grpcpp/grpcpp.h>

#include <algorithm>

#include "convert.hpp"
#include "types.hpp"

namespace abeille {
namespace raft_node {

RaftPool::RaftPool(RaftPool::RaftRef raft) : raft_(raft) {
  // FIXME: It's not ok. Maybe singleton global Config
  auto peers_adresses = raft_->core_->config_.GetPeers();

  for (const auto& addr : peers_adresses) {
    ServerId temp_id = address2uint(addr);
    peers_.insert(std::make_pair(
        temp_id,
        std::make_shared<Peer>(
            grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()),
            raft_, temp_id)));
  }
}

RaftPool::Status RaftPool::Run() {
  for (auto& peer : peers_) {
    peer.second->Run(peer.second);
  }

  return Status();
}

void RaftPool::BeginRequestVote() {
  for (auto& peer : peers_) peer.second->BeginRequestVote();
}

bool RaftPool::MajorityVotes() {
  uint64_t votes_num =
      std::count_if(peers_.cbegin(), peers_.cend(),
                    [](const std::pair<ServerId, RaftPool::PeerRef> peer) {
                      return peer.second->HaveVote();
                    });

  bool vote_yourself = raft_->voted_for_ == raft_->id_;
  return votes_num + vote_yourself > peers_.size() / 2;
}

void RaftPool::AppendAll(const Entry& entry) { raft_->log_->Append(entry); }

void RaftPool::Shutdown() {
  for (auto& peer : peers_) peer.second->Shutdown();
}

}  // namespace raft_node
}  // namespace abeille
