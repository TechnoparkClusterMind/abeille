#ifndef ABEILLE_RAFT_RAFT_H_
#define ABEILLE_RAFT_RAFT_H_

#include <chrono>
#include <memory>
#include <thread>
#include <condition_variable>

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

public: // Methods
  RaftConsensus() = default;
  // Constructor
  explicit RaftConsensus(Core* core);

  // Destrictor
  ~RaftConsensus();

  // process an append_entries RPC from another server
  void HandleAppendEntries(const AppendEntryRequest *msg,
                          AppendEntryResponse *resp);

  // process a request_vote RPC from another server
  void HandleRequestVote(const RequestVoteRequest *msg,
                         RequestVoteResponse *resp);

  // start timer and peer threads
  void Run();

  void Shutdown();

private: // Methods
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
  void appendEntry(Peer& peer);
  void requestVote(Peer& peer);

public:
  State state_;
  uint64_t id_;
  uint64_t leader_id_;

private:
  typedef std::chrono::milliseconds timeDuration;
  typedef std::chrono::steady_clock Clock;
  typedef Clock::time_point timePoint;


  // in order to get the current time
  Clock clock_;

  // when the next heartbeat should be sent
  timePoint heartbeat_period_;

  // randomized time point for every server
  timePoint election_timeout_;

  // the time at which timerThreadMain() should start a new election
  timePoint start_new_election_at_;

  std::unique_ptr<Log> log_;

  // this thread executes timer_thread_main
  // it begins new eleciton if needed
  std::thread timer_thread_;

  // the latest term this server has seen
  uint64_t current_term_;

  // Index of the last commited Entry
  uint64_t commit_index;

  // the server id that this server voted for during this term
  // if 0 then no vote has been given
  uint64_t voted_for_;

  // is raft exiting
  bool exiting_;

  Core* core_;

  std::atomic<uint64_t>* num_peers_threads_;

  friend class Peer;
};

} // namespace raft_node
} // namespace abeille

#endif //  ABEILLE_RAFT_RAFT_H_
