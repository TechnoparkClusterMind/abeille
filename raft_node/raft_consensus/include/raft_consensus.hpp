#ifndef ABEILLE_RAFT_RAFT_H_
#define ABEILLE_RAFT_RAFT_H_

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "abeille.pb.h"
#include "config.hpp"
#include "log.hpp"
#include "peer.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class Peer;

class RaftConsensus {
private:
  enum class State {
    FOLLOWER,
    CANDIDATE,
    LEADER,
  };

public: // Methods
  // Constructor
  // FIXME: do i need pool
  explicit RaftConsensus();

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

private: // Methods
  // Initiate RPCs if needed
  void peerThreadMain(std::shared_ptr<Peer> peer);

  // Starts new election when it's time
  void timerThreadMain();

  // Start election process
  void startNewElection();

public:
  // FIXME: do I need to store config
  State state_;

  // The number of peer's thread currently active
  std::atomic<uint64_t> num_peers_thread_;

private:
  typedef std::chrono::time_point<std::chrono::steady_clock> timePoint;
  std::unique_ptr<Log> log_;

  // this thread executes timer_thread_main
  // it begins new eleciton if needed
  std::thread timer_thread_;

  // shows when the next heartbeat should be sent
  timePoint heartbeat_time_;

  // randomized time point for every server
  timePoint start_election_at_;

  // the latest term this server has seen
  uint64_t current_term_;

  // the server id that this server voted for during this term
  // if 0 then no vote has been given
  uint64_t voted_for_;

  // in order to read the current time
  std::chrono::steady_clock clock_;

  friend class Peer;
};

} // namespace raft_node
} // namespace abeille

#endif //  ABEILLE_RAFT_RAFT_H_
