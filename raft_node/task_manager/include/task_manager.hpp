#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <memory>
#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"

namespace abeille {
namespace raft_node {

class TaskManager {
public:
  TaskManager() = default;
  ~TaskManager() = default;

<<<<<<< HEAD:task_manager/include/task_manager.hpp
  void Run();

 private:
=======
private:
  std::thread scheduler_thread_;
>>>>>>> dev_serpent:raft_node/task_manager/include/task_manager.hpp
  std::queue<Entry> queued_entries_;
  std::unique_ptr<std::thread> scheduler_thread_ptr_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_MANAGER_H
