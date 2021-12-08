#ifndef ABEILLE_RAFT_STATE_MACHINE_H_
#define ABEILLE_RAFT_STATE_MACHINE_H_

#include "log.hpp"
#include "types.hpp"

namespace abeille {
namespace raft_node {

class StateMachine {
 public:
  explicit StateMachine(std::shared_ptr<Log> log) : log_(log) {}
  Index GetCommitIndex() const noexcept { return commit_index_; }
  void Commit(Index idx);

 private:
  // Index of the last commited entry
  Index commit_index_ = 0;
  std::shared_ptr<Log> log_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_STATE_MACHINE_H_
