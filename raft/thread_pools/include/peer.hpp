#ifndef ABEILLE_RAFT_PEER_H_
#define ABEILLE_RAFT_PEER_H_

#include <grpcpp/channel.h>

#include <chrono>
#include <memory>
#include <queue>
#include <thread>

#include "raft/consensus/include/raft_consensus.hpp"
#include "raft/rpc/include/raft_service.hpp"
#include "rpc/proto/abeille.pb.h"
#include "utils/include/types.hpp"

namespace abeille {
namespace raft {

// forward declaration
class RaftConsensus;

// Tracks various bits of state for each server, wich is used
// when we are the candidate or the leader
class Peer {
 public:
  using TimePoint = std::chrono::steady_clock::time_point;

  // grpc stub is initialized with channel
  explicit Peer(std::shared_ptr<grpc::Channel> channel,
                std::shared_ptr<RaftConsensus> raft, uint64_t id) noexcept;

  ~Peer() = default;

  // Run peer_thread_main from Raft_consensus
  void Run(std::shared_ptr<Peer> self) noexcept;
  void Shutdown() noexcept { exiting_ = true; }
  void BeginRequestVote() noexcept;
  void UpdatePeer() noexcept;
  bool HaveVote() const noexcept { return have_vote_; }
  Index const& GetMatchIndex() const noexcept { return match_index_; }

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
  bool exiting_ = false;

  bool vote_request_done_ = false;
  bool have_vote_ = false;

  TimePoint next_heartbeat_time_ = TimePoint::min();
  Index next_index_;
  Index match_index_ = 0;

  friend class RaftConsensus;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_PEER_H_
