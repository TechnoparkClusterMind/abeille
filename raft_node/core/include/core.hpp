#ifndef ABEILLE_RAFT_CORE_H_
#define ABEILLE_RAFT_CORE_H_

#include <atomic>
#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <memory>

#include "config.hpp"
#include "raft_consensus.hpp"
#include "raft_pool.hpp"
#include "server.hpp"
#include "task_manager.hpp"
#include "worker_pool.hpp"

namespace abeille {
namespace raft_node {

// Holds the Abeille raft server's daemon's top-level objects
// The purpose of main() is to create and run a Core object
class Core {
public:
  // Initialize this object, Raft_consensus and Task_manager with config
  // Config must be checked before been passed to init()
  explicit Core(Config &&conf);

  // Default destructor
  ~Core();

  // Run Raft_consensus, Task_manager and Raft_node::RPC::Server
  void Run();

  // TODO: sophisticated signal handlers
  // used by signal() in main()
  // maybe we'll need other methods of signal processing
  // (not so primitive one)
  void HandleSignal(int signum);

  void Shutdown();

  // Non-copyable object
  Core(const Core &) = delete;
  Core &operator=(const Core &) = delete;

public:
  uint64_t server_id_;

  // Triggered when anything changes
  std::condition_variable state_changed_;
  std::mutex mutex;

private:
  // Raft_node::Config
  Config config_;

  // Making outbound RPCs to Raft_nodes
  std::shared_ptr<RaftPool> raft_pool_;

  // Making outbound RPCs to Raft_nodes
  std::shared_ptr<WorkerPool> worker_pool_;

  std::shared_ptr<RaftConsensus> raft_;
  std::shared_ptr<TaskManager> task_mgr_;

  // Listens for inbound RPCs from raft nodes and pass them to the services
  std::unique_ptr<abeille::rpc::Server> raft_server_;

  // Listens for inbound RPCs from user ...
  std::unique_ptr<abeille::rpc::Server> user_server_;

  // The number of peer's thread currently active
  std::atomic<uint64_t> num_peers_threads_;

  // The number of worker's thread currently active
  std::atomic<uint64_t> num_workers_threads_;

  std::unique_ptr<grpc::Service> raft_service_;
  std::unique_ptr<grpc::Service> user_service_;

  // Is core exiting
  bool exiting_;

  friend class RaftConsensus;
  friend class Peer;
  friend class Worker;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_CORE_H_
