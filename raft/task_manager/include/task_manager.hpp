#ifndef ABEILLE_RAFT_TASK_MANAGER_HPP_
#define ABEILLE_RAFT_TASK_MANAGER_HPP_

#include <memory>
#include <queue>
#include <thread>

#include "raft/core/include/core.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/errors.hpp"
#include "utils/include/types.hpp"

namespace abeille {
namespace raft {

// forward declaration
class Core;

class TaskManager {
 public:
  TaskManager() = default;
  explicit TaskManager(Core *core) : core_(core) {}
  ~TaskManager() = default;

  error UploadTaskData(const Bytes &task_data, const TaskID &task_id);
  error ProcessTask(const TaskWrapper &task_wrapper);

 private:
  Core *core_;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_TASK_MANAGER_HPP_
