#ifndef ABEILLE_RAFT_RAFT_H_
#define ABEILLE_RAFT_RAFT_H_

#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>

#include "abeille.pb.h"
#include "core.hpp"
#include "errors.hpp"
#include "log.hpp"
#include "peer.hpp"
#include "state_machine.hpp"
#include "types.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class Peer;
class Core;

class RaftConsensus {
 public:
  enum class State { FOLLOWER, CANDIDATE, LEADER };

  typedef std::shared_ptr<Log> LogRef;
  // FIXME: refactor Core*
  // typedef std::shared_ptr<Core> CoreRef;
  typedef Core* CoreRef;
  typedef std::unique_ptr<StateMachine> StateMachineRef;
  typedef error Status;

  typedef std::chrono::system_clock::time_point TimePoint;
  typedef std::chrono::milliseconds TimeDuration;
  typedef std::chrono::system_clock Clock;


  RaftConsensus() = default;
  explicit RaftConsensus(Core *core) noexcept;
  ~RaftConsensus();

  // process RPCs from another server
  void HandleAppendEntry(const AppendEntryRequest *msg,
                           AppendEntryResponse *resp);
  void HandleRequestVote(const RequestVoteRequest *msg,
                         RequestVoteResponse *resp);

  // Starts shutdown timerThreadMain
  Status Run();
  void Shutdown() noexcept;

  // Interface for communication with Core
  void AddPeer() noexcept;

 private:  // Methods
  // Main logic for every node depending on current state of server
  void peerThreadMain(std::shared_ptr<Peer> peer);

  // For election process:
  void timerThreadMain();
  void startNewElection();
  void resetElectionTimer();
  void becomeLeader();

  // Update term, convert to follower,
  void stepDown(uint64_t term);

  // RPC request
  void appendEntry(std::unique_lock<std::mutex>&, Peer &peer);
  void requestVote(std::unique_lock<std::mutex>&, Peer &peer);

 private:

  uint64_t id_ = 0;
  uint64_t leader_id_ = 0;
  State state_ = State::FOLLOWER;

  // Triggers when almost anything inside raft happens
  std::condition_variable raft_state_changed_;
  mutable std::mutex mutex_;

  TimeDuration ELECTION_TIMEOUT_= (std::chrono::milliseconds(500));

  // when the next heartbeat should be sent
  TimeDuration HEARTBEAT_PERIOD_ = std::chrono::milliseconds(
      ELECTION_TIMEOUT_.count() / 2);

  // the time at which timerThreadMain() should start a new election
  TimePoint start_new_election_at_ = TimePoint::max();

  // don't respond to request votes until
  TimePoint hold_election_for = TimePoint::min();

  CoreRef core_ = nullptr;
  LogRef log_;
  StateMachineRef state_machine_;

  // this thread executes timer_thread_main
  // it begins new eleciton if needed
  std::thread timer_thread_;

  // the server id that this server voted for during this term
  // if 0 then no vote has been given
  Index voted_for_ = 0;

  // is raft exiting
  bool shutdown_ = false;

  // the latest term this server has seen
  Term current_term_ = 0;

  friend class RaftPool;
  friend class Peer;
};

}  // namespace raft_node
}  // namespace abeille

#endif  //  ABEILLE_RAFT_RAFT_H_
