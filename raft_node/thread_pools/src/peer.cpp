#include "peer.hpp"
#include "macroses.hpp"
#include "abeille.pb.h"
#include "raft_consensus.hpp"

// Must be replace with the logging system in the future

namespace abeille {
namespace raft_node {

Peer::Peer(std::shared_ptr<grpc::Channel> channel, std::shared_ptr<RaftConsensus> raft, uint64_t id)
    : id_(id), stub_(RaftService::NewStub(channel)), raft_(raft) {}

void Peer::Run(std::shared_ptr<Peer> self) {
  ++raft_->num_peers_threads_;
  LOG("Starting peer thread for server %lu", id_);
  std::thread(&RaftConsensus::peerThreadMain, raft_, self).detach();
}

}  // namespace raft_node
}  // namespace abeille
