#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"

namespace abeille {
namespace raft_node {

class TaskManager {
public:
  TaskManager() = default;
  ~TaskManager() = default;

private:
  std::thread scheduler_thread_;
  std::queue<Entry> queued_entries_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_MANAGER_H
