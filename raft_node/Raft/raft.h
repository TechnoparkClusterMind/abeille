#ifndef ABEILLE_RAFT_RAFT_H_
#define ABEILLE_RAFT_RAFT_H_

#include <chrono>
#include <thread>

#include "raft_client.h"
#include "raft_service.pb.h"

namespace Abeille {
namespace Raft_node {
class Raft_consensus {
   private:
    enum class State {
        FOLLOWER,
        CANDIDATE,
        LEADER,
    };

   public:  // Methods
    // Constructor
    explicit Raft_consensus(std::shared_ptr<Raft_client> client);

    // Destrictor
    ~Raft_consensus();

    // process an append_entries RPC from another server
    void handle_append_entry(const Proto::Raft_service::append_entry_msg* msg,
                             Proto::Raft_service::append_entry_response* resp);

    // process a request_vote RPC from another server
    void handle_request_vote(const Proto::Raft_service::request_vote_msg* msg,
                             Proto::Raft_service::request_vote_response* resp);

    // start timer and peer's threads
    void run_threads();

   private:  // Methods
    // Initiate RPCs if needed
    void peer_thread_main(Peer& peer);

    // Starts new election when it's time
    void timer_thread_main();

    // Start election process
    void start_new_election();

   public:  // Attributes
    Config& conf;
    State state;

   private:  // Attributes
    typedef std::chrono::time_point<std::chrono::steady_clock> time_point;

    std::unique_ptr<Log> log;

    // this thread executes timer_thread_main
    // it begins new eleciton if needed
    std::thread timer_thread;

    // shows when the next heartbeat should be sent
    time_point heartbeat_time;

    // randomized time point for every server
    time_point start_election_at;

    // the latest term this server has seen
    uint64_t current_term;

    // the server id that this server voted for during this term
    // if 0 then no vote has been given
    uint64_t voted_for;

    // in order to read the current time
    std::chrono::steady_clock clock;

    friend class Peer;
};
}  // namespace Raft_node
}  // namespace Abeille

#endif  //  ABEILLE_RAFT_RAFT_H_
