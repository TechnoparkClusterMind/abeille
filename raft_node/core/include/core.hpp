#ifndef ABEILLE_RAFT_CORE_H_
#define ABEILLE_RAFT_CORE_H_

#include <grpcpp/grpcpp.h>

#include <atomic>
#include <condition_variable>
#include <memory>

#include "config.hpp"
#include "errors.hpp"
#include "raft_consensus.hpp"
#include "raft_pool.hpp"
#include "raft_service.hpp"
#include "server.hpp"
#include "task_manager.hpp"
#include "worker_pool.hpp"
#include "user_service.hpp"
#include "worker_service.hpp"

using abeille::rpc::Server;

namespace abeille {
namespace raft_node {

// forward declaration
class TaskManager;
class RaftPool;
class RaftServiceImpl;
class UserServiceImpl;
class WorkerServiceImpl;

// FIXME: refactor to singleton
// Holds the Abeille raft server's daemon's top-level objects
// The purpose of main() is to create and run a Core object
class Core {
 public:
  typedef std::shared_ptr<RaftConsensus> RaftRef;
  typedef std::shared_ptr<TaskManager> TaskMgrRef;
  typedef std::unique_ptr<grpc::Service> ServiceRef;
  typedef std::unique_ptr<RaftPool> RaftThreadPoolRef;
  typedef std::unique_ptr<WorkerPool> WorkerThreadPoolRef;
  typedef std::unique_ptr<Server> ServerRef;
  typedef std::atomic<uint64_t> ThreadsNum;
  typedef std::shared_ptr<Core> This;
  typedef error Status;

  // Constructors:
  //  * Default constructor initializes all pointer with nullptr
  //  * Explicit one uses config in order to initialize itself & server
  //    with config. Config is also used for thread pool initialization
  Core() = default;
  explicit Core(Config &&conf) noexcept;
  Core(Core &&other) = delete;

  // Default destructor:
  //  * just calls Shutdown method
  ~Core();

  // Run():
  //  * launches all threads from every top-level object
  //  Shutdown():
  //  * launches Shutdown from all top-level objects
  //  * waits untill all pool thread will be closed
  void Run() noexcept;
  void Shutdown() noexcept;

  // Non-copyable object
  Core(const Core &) = delete;
  Core &operator=(const Core &) = delete;

 public:
  uint64_t server_id_ = 0;
  // In order to track the state of the peer and worker threads
  std::condition_variable pool_state_changed_;
  std::mutex mutex;

 private:
  // Pretty self-explanatory
  Config config_;
  RaftRef raft_ = nullptr;
  TaskMgrRef task_mgr_ = nullptr;

  // Making outbound RPCs to Raft_nodes
  RaftThreadPoolRef raft_pool_ = nullptr;
  WorkerThreadPoolRef worker_pool_ = nullptr;

  // Listens for inbound RPCs from raft nodes and pass them to the services
  ServerRef raft_server_ = nullptr;

  // It's better to store services here and not to rely on grpc
  ServiceRef raft_service_ = nullptr;
  ServiceRef user_service_ = nullptr;
  ServiceRef worker_service_ = nullptr;

  // The number of peer's / worker's thread currently active
  ThreadsNum num_peers_threads_ = 0;
  ThreadsNum num_workers_threads_ = 0;

  // Is core being shutting down
  bool shutdown_ = false;

  friend class RaftConsensus;
  friend class RaftPool;
  friend class Peer;
  friend class Worker;
  friend class TaskManager;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_CORE_H_
