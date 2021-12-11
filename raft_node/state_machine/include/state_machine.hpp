#ifndef ABEILLE_RAFT_STATE_MACHINE_H_
#define ABEILLE_RAFT_STATE_MACHINE_H_

#include <unordered_map>

#include "errors.hpp"
#include "log.hpp"
#include "types.hpp"
#include "core.hpp"

namespace abeille {
namespace raft_node {

// Forward declaration
class Core;

class StateMachine {
  typedef Core* CorePointer;
  typedef error Status;

 public:
  explicit StateMachine(CorePointer core) : core_(core) {}
  Index GetCommitIndex() const noexcept { return commit_index_; }
  void Commit(Index index) noexcept;

 private:
  Status applyCommand(const Log::EntryConstReference entry) noexcept;

  // Reaching consensus on this data:
  std::unordered_map<TaskId, Task> assigned_;
  std::unordered_map<TaskId, Task> completed_;

  // Index of the last commited entry
  Index commit_index_ = 0;
  // for accessing log and task_mgr
  CorePointer core_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_STATE_MACHINE_H_
