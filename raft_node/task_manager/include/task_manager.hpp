#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <memory>
#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"
#include "core.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class Core;

class TaskManager {
public:
  TaskManager() = default;
  explicit TaskManager(Core* core);
  ~TaskManager() = default;

  void Run();
  void Shutdown();

private:
  std::thread scheduler_thread_;
  std::queue<Entry> queued_entries_;
  std::unique_ptr<std::thread> scheduler_thread_ptr_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_MANAGER_H
