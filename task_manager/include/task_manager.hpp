#ifndef ABEILLE_RAFT_MANAGER_H
#define ABEILLE_RAFT_MANAGER_H

#include <memory>
#include <queue>
#include <thread>

#include "abeille.grpc.pb.h"

class TaskManager {
 public:
  TaskManager() = default;
  ~TaskManager() = default;

  void Run();

 private:
  std::queue<Entry> queued_entries_;
  std::unique_ptr<std::thread> scheduler_thread_ptr_;
};

#endif  // ABEILLE_RAFT_MANAGER_H
