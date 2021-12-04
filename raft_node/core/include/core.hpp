#ifndef ABEILLE_RAFT_CORE_H_
#define ABEILLE_RAFT_CORE_H_

#include <grpcpp/grpcpp.h>

#include <atomic>
#include <condition_variable>
#include <memory>

#include "config.hpp"
#include "raft_consensus.hpp"
#include "raft_pool.hpp"
#include "server.hpp"
#include "task_manager.hpp"

using abeille::rpc::Server;

namespace abeille {
namespace raft_node {

class TaskManager;

// Holds the Abeille raft server's daemon's top-level objects
// The purpose of main() is to create and run a Core object
class Core {
 public:
  Core() = default;
  // Initialize this object, RaftConsensus and TaskManager with config
  // Config must be checked before been passed to init()
  explicit Core(Config &&conf) noexcept;

  Core(Core &&other) noexcept;

  // Default destructor
  ~Core();

  // Run RaftConsensus, TaskManager and RaftNode::RPC::Server
  void Run();

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
  // RaftNode::Config
  Config config_;

  std::shared_ptr<RaftConsensus> raft_ = nullptr;

  std::shared_ptr<TaskManager> task_mgr_ = nullptr;

  // Making outbound RPCs to Raft_nodes
  std::shared_ptr<RaftPool> raft_pool_ = nullptr;

  std::unique_ptr<grpc::Service> raft_service_ = nullptr;

  std::unique_ptr<grpc::Service> user_service_ = nullptr;

  // Listens for inbound RPCs from raft nodes and pass them to the services
  std::unique_ptr<Server> raft_server_ = nullptr;

  // The number of peer's thread currently active
  std::atomic<uint64_t> num_peers_threads_ = 0;

  // The number of worker's thread currently active
  std::atomic<uint64_t> num_workers_threads_ = 0;

  // Is core being shutdown
  bool shutdown_ = false;

  friend class RaftConsensus;
  friend class Peer;
  friend class Worker;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_CORE_H_
