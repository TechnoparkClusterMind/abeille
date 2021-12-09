#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <memory>
#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"
#include "core.hpp"
#include "errors.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class Core;

class TaskManager {
 public:
  TaskManager() = default;
  // FIXME: not done yet
  explicit TaskManager(Core *core) : core_(core) {}
  ~TaskManager() = default;

  error UploadData(const TaskData &task_data, uint64_t &task_id);

  // FIXME: not implemented
  void Run(){};
  void Shutdown(){};

 private:
  std::thread scheduler_thread_;
  std::queue<Entry> queued_entries_;
  std::unique_ptr<std::thread> scheduler_thread_ptr_;
  uint64_t last_task_id_ = 1;
  Core *core_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_MANAGER_H
