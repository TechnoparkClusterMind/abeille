#include "raft_pool.hpp"

#include <grpcpp/grpcpp.h>

#include "convert.hpp"
#include "types.hpp"

namespace abeille {
namespace raft_node {

RaftPool::RaftPool(RaftPool::RaftRef raft) : raft_(raft) {
  // FIXME: It's not ok. Maybe singleton global Config
  auto peers_adresses = raft_->core->config.GetPeers();

  for (const auto& addr : peers_adresses) {
    ServerId temp_id = address2uint(addr);

    peers_.insert(std::make_pair(
        temp_id,
        std::make_shared<Peer>(
            grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()),
            raft_, temp_id)));
  }
}

void RaftPool::Run() {
  for (auto& peer : peers_) {
    peer.second->Run(peer.second);
  }
}

void RaftPool::Shutdown() {
  for (auto& peer : peers_) peer.second->Shutdown();
}

}  // namespace raft_node
}  // namespace abeille
