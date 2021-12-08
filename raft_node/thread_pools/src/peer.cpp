#include "peer.hpp"

#include "abeille.pb.h"
#include "logger.hpp"
#include "raft_consensus.hpp"

// Must be replace with the logging system in the future

namespace abeille {
namespace raft_node {

Peer::Peer(std::shared_ptr<grpc::Channel> channel,
           std::shared_ptr<RaftConsensus> raft, uint64_t id)
    : id_(id),
      stub_(RaftService::NewStub(channel)),
      raft_(raft),
      next_index_(raft_->log_->LastIndex() + 1) {}

void Peer::Run(std::shared_ptr<Peer> self) {
  LOG_INFO("Starting peer thread for server %lu", id_);
  raft_->AddPeer();
  std::thread(&RaftConsensus::peerThreadMain, raft_, self).detach();
}

void Peer::BeginRequestVote() noexcept {
  vote_request_done_ = false;
  have_vote_ = false;
}

}  // namespace raft_node
}  // namespace abeille
