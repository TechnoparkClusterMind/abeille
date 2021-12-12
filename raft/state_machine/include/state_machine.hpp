#ifndef ABEILLE_RAFT_STATE_MACHINE_H_
#define ABEILLE_RAFT_STATE_MACHINE_H_

#include <functional>
#include <unordered_map>

#include "raft/core/include/core.hpp"
#include "raft/log/include/log.hpp"
#include "utils/include/errors.hpp"

namespace abeille {
namespace raft {

// Forward declaration
class Core;

struct TaskIDWrapper {
  TaskIDWrapper(const std::string &filename, uint64_t client_id) noexcept
      : filename_(filename), client_id_(client_id) {}

  bool operator==(const TaskIDWrapper &other) const noexcept {
    return filename_ == other.filename_ && client_id_ == other.client_id_;
  }

  struct Hash {
    size_t operator()(const TaskIDWrapper &tw) const noexcept {
      return std::hash<std::string>{}(tw.filename_ + std::to_string(tw.client_id_));
    }
  };

  std::string filename_;
  uint64_t client_id_ = 0;
};

class StateMachine {
 public:
  using HashTable = std::unordered_map<TaskIDWrapper, TaskWrapper, TaskIDWrapper::Hash>;

  explicit StateMachine(Core *core) : core_(core) {}
  Index GetCommitIndex() const noexcept { return commit_index_; }
  void Commit(Index index) noexcept;

 private:
  error applyCommand(const Log::EntryConstReference entry) noexcept;

  // error handleCommandAdd()

  // Reaching consensus on this data:
  HashTable assigned_;
  HashTable completed_;

  // Index of the last commited entry
  Index commit_index_ = 0;
  // for accessing log and task_mgr
  Core *core_;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_STATE_MACHINE_H_
