#ifndef ABEILLE_RAFT_CORE_H_
#define ABEILLE_RAFT_CORE_H_

#include <memory>

#include "raft/raft.h"
#include "raft_client.h"
#include "raft_server.h"

namespace Abeille {
namespace Raft_node {

// Holds the Abeille raft server's daemon's top-level objects
// The purpose of main() is to create and run a Core object
class Core {
   public:
    // Initialize this object, Raft_consensus and Task_manager with config
    // Config must be checked before been passed to init()
    explicit Core(Config& conf);

    // Default destructor
    ~Core();

    // Run Raft_consensus, Task_manager and Raft_node::RPC::Server
    void run();

    // TODO: sophisticated signal handlers
    // used by signal() in main()
    // maybe we'll need other methods of signal processing
    // (not so primitive one)
    void handle_signal(int signum);

    // Non-copyable object
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

   public:
    uint64_t server_id;

   private:
    // Making outbound RPCs to Raft_nodes
    std::shared_ptr<Raft_client> raft_client;

    // Making outbound RPCs to Raft_nodes
    std::shared_ptr<Worker_client> worker_client;

    std::shared_ptr<Raft_consensus> raft;
    std::shared_ptr<Task_manager> task_mgr;

    // Listens for inbound RPCs from raft nodes and pass them to the services
    std::unique_ptr<Server> raft_server;

    // Listens for inbound RPCs from user ...
    std::unique_ptr<Server> user_server;

    // Services for servers
    std::unique_ptr<Raft_service> raft_service;
    std::unique_ptr<User_service> user_service;

    // Raft_node::Config
    Config config;
};

}  // namespace Raft_node
}  // namespace Abeille

#endif  // ABEILLE_RAFT_CORE_H_
