#ifndef ABEILLE_RAFT_WORKER_POOL_H_
#define ABEILLE_RAFT_WORKER_POOL_H_
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "abeille.pb.h"
#include "raft_consensus.hpp"
#include "task_manager.hpp"
#include "worker.hpp"

namespace abeille {
namespace raft_node {

class TaskManager;

class WorkerPool {
public:
  WorkerPool(std::shared_ptr<TaskManager> task_mgr) : task_mgr_(task_mgr) {}
  // FIXME: Not implemented
  void Run(){};
  void Shutdown(){};

private:
  std::shared_ptr<TaskManager> task_mgr_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_WORKER_POOL_H_
