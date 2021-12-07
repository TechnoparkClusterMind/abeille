#ifndef ABEILLE_RAFT_PEER_H_
#define ABEILLE_RAFT_PEER_H_

#include <grpcpp/channel.h>

#include <chrono>
#include <memory>
#include <queue>
#include <thread>

#include "abeille.pb.h"
#include "node.hpp"
#include "raft_consensus.hpp"
#include "raft_service.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class RaftConsensus;

// Tracks various bits of state for each server, wich is used
// when we are the candidate or the leader
class Peer {
 public:
  using timePoint = std::chrono::steady_clock::time_point;

  // grpc stub is initialized with channel
  explicit Peer(std::shared_ptr<grpc::Channel> channel, std::shared_ptr<RaftConsensus> raft, uint64_t id);

  // destructor
  ~Peer();

  // Run peer_thread_main from Raft_consensus
  void Run(std::shared_ptr<Peer> self);

  // id of the current raft_node
  uint64_t id_;

  // to this queue entries are pushed (from task_manager)
  std::queue<Entry> entries_;

 private:
  // for RPCs
  std::unique_ptr<RaftService::Stub> stub_;
  std::shared_ptr<RaftConsensus> raft_;

  std::thread peer_thread_;

  // for peer_thread_main to know when shutdown
  bool exiting_;

  bool vote_request_done_;
  bool have_vote_;

  timePoint next_heartbeat_time_;
  uint64_t next_index_;

  friend class RaftConsensus;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_PEER_H_
