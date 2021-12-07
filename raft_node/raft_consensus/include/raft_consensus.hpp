#ifndef ABEILLE_RAFT_RAFT_H_
#define ABEILLE_RAFT_RAFT_H_

#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>

#include "abeille.pb.h"
#include "core.hpp"
#include "log.hpp"
#include "peer.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class Peer;
class Core;

class RaftConsensus {
 private:
  enum class State {
    FOLLOWER,
    CANDIDATE,
    LEADER,
  };

 public:  // Methods
  RaftConsensus() = default;
  // Constructor
  explicit RaftConsensus(Core *core) noexcept;

  // Destrictor
  ~RaftConsensus();

  // process an append_entries RPC from another server
  void HandleAppendEntries(const AppendEntryRequest *msg, AppendEntryResponse *resp);

  // process a request_vote RPC from another server
  void HandleRequestVote(const RequestVoteRequest *msg, RequestVoteResponse *resp);

  // start timer and peer threads
  void Run();

  void Shutdown() noexcept;

 private:  // Methods
  // Initiate RPCs if needed
  void peerThreadMain(std::shared_ptr<Peer> peer);

  // Starts new election when it's time
  void timerThreadMain();

  // Start election process
  void startNewElection();

  void resetElectionTimer();

  void becomeLeader();

  // return to the follower state
  void stepDown(uint64_t term);

  // RPC request
  void appendEntry(Peer &peer);
  void requestVote(Peer &peer);

 private:
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::steady_clock::time_point;
  using TimeDuration = std::chrono::milliseconds;

  uint64_t id_ = 0;
  uint64_t leader_id_ = 0;
  State state_ = State::FOLLOWER;

  // in order to get the current time
  Clock clock_;

  // randomized time point for every server
  TimePoint election_timeout_ = TimePoint(std::chrono::milliseconds(500));

  // when the next heartbeat should be sent
  TimePoint heartbeat_period_ = TimePoint(std::chrono::milliseconds(election_timeout_.time_since_epoch().count() / 2));

  // the time at which timerThreadMain() should start a new election
  TimePoint start_new_election_at_ = TimePoint::max();

  std::unique_ptr<Log> log_;

  // this thread executes timer_thread_main
  // it begins new eleciton if needed
  std::thread timer_thread_;

  // the latest term this server has seen
  uint64_t current_term_ = 0;

  // Index of the last commited Entry
  uint64_t commit_index = 0;

  // the server id that this server voted for during this term
  // if 0 then no vote has been given
  uint64_t voted_for_ = 0;

  // is raft exiting
  bool shutdown_ = false;

  Core *core_ = nullptr;

  std::atomic<uint64_t> *num_peers_threads_ = nullptr;

  friend class Peer;
};

}  // namespace raft_node
}  // namespace abeille

#endif  //  ABEILLE_RAFT_RAFT_H_
