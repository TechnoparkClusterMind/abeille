#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"

class TaskManager {
 public:
  TaskManager() = default;
  ~TaskManager() = default;

 private:
  std::thread scheduler_thread_;
  std::queue<Entry> queued_entries_;
};

#endif  // ABEILLE_RAFT_MANAGER_H
