#ifndef ABEILLE_RAFT_TASK_MANAGER_HPP_
#define ABEILLE_RAFT_TASK_MANAGER_HPP_

#include <memory>
#include <queue>
#include <thread>

#include "raft/core/include/core.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/errors.hpp"

namespace abeille {
namespace raft {

// forward declaration
class Core;

class TaskManager {
 public:
  TaskManager() = default;
  explicit TaskManager(Core *core) : core_(core) {}
  ~TaskManager() = default;

  error UploadData(const TaskData &task_data, uint64_t &task_id);
  error ProcessTask(const Task &task);

 private:
  Core *core_;
  uint64_t last_task_id_ = 1;  // TODO: remove in big refactor
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_TASK_MANAGER_HPP_
