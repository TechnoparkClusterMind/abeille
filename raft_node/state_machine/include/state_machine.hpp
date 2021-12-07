#ifndef ABEILLE_RAFT_STATE_MACHINE_H_
#define ABEILLE_RAFT_STATE_MACHINE_H_

#include "types.hpp"

namespace abeille {
namespace raft_node {

class StateMachine {
 public:
  Index GetCommitIndex() const noexcept { return commit_index_; }

 private:
  // Index of the last commited entry
  Index commit_index_ = 0;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RAFT_STATE_MACHINE_H_
