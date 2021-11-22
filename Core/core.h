#ifndef ABEILLE_RAFT_CORE_H_
#define ABEILLE_RAFT_CORE_H_
#include <memory>

namespace Abeille {
namespace Raft_node {

// Holds the Abeille raft server's daemon's top-level objects
// The purpose of main() is to create and run a Core object
class Core {
public:
  // Default constructor
  Core();
  // Default destructor
  ~Core();

  // Initialize this object, Raft_consensus and Task_manager with config
  // Config must be checked before been passed to init()
  void init(Raft_node::Config &conf);

  // Run Raft_consensus, Task_manager and Raft_node::RPC::Server
  void run();

  // TODO: sophisticated signal handlers
  //
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
  std::shared_ptr<Raft_node::Raft_consensus> raft;
  std::shared_ptr<Raft_node::Task_manager> task_mgr;
  // Listens for inbound RPCs from raft nodes and pass them to the services
  std::unique_ptr<Raft_node::RPC::Raft_server> raft_server;
  // Listens for inbound RPCs from user ...
  std::unique_ptr<Raft_node::RPC::User_server> user_server;
};

} // namespace Raft_node
} // namespace Abeille

#endif // ABEILLE_RAFT_CORE_H_
