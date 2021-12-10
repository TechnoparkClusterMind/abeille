#include "state_machine.hpp"

#include "logger.hpp"

namespace abeille {
namespace raft_node {

    void StateMachine::Commit(Index idx) {
        LOG_TRACE();
        ++commit_index_;
    }

}  // namespace raft_node
}  // namespace abeille
