#include "raft/state_machine/include/state_machine.hpp"

#include "utils/include/logger.hpp"

#define ASSIGNED TaskStatus::TASK_STATUS_ASSIGNED;
#define COMPLETED TaskStatus::TASK_STATUS_COMPLETED;

namespace abeille {
namespace raft {

void StateMachine::Commit(Index index) noexcept {
  // log will be refactored to singleton
  assert(core_->raft_->log_->LastIndex() >= index);
  assert(index - commit_index_ >= 1);

  for (Index idx = commit_index_ + 1; idx <= index; ++idx) {
    auto status = applyCommand((*core_->raft_->log_)[idx]);
    if (!status.ok()) {
      LOG_ERROR("Error while applying commited entry");
    }
  }
  commit_index_ = index;
  // LOG_DEBUG("StateMachine: %lu - %lu", assigned_.size(), completed_.size());
}

StateMachine::Status StateMachine::applyCommand(
    const Log::EntryConstReference entry) noexcept {
  auto task_id = entry.task().id();
  auto task_pair = std::make_pair(task_id, entry.task());
  auto failure = Status(Status::Code::FAILURE);

  if (entry.command() == RaftCommand::RAFT_COMMAND_ADD) {
    auto add_task_status = entry.add_request().to();
    if (add_task_status == TaskStatus::TASK_STATUS_ASSIGNED) {
      assigned_.insert(task_pair);
      if (core_->raft_->IsLeader()) {
        LOG_TRACE();
        core_->task_mgr_->ProcessTask(entry.task());
      }
    } else if (add_task_status == TaskStatus::TASK_STATUS_COMPLETED) {
      completed_.insert(task_pair);
    } else {
      LOG_ERROR("Unknown add_to task status");
      return failure;
    }
  } else if (entry.command() == RaftCommand::RAFT_COMMAND_MOVE) {
    auto move_to_task_status = entry.move_request().to();
    auto move_from_task_status = entry.move_request().from();

    if (move_from_task_status == TaskStatus::TASK_STATUS_ASSIGNED) {
      assigned_.erase(task_id);
    } else if (move_from_task_status == TaskStatus::TASK_STATUS_COMPLETED) {
      LOG_WARN("Deleting completed task from state machine");
      completed_.erase(task_id);
    } else {
      LOG_ERROR("Unknown move_from task status");
      return failure;
    }

    if (move_to_task_status == TaskStatus::TASK_STATUS_ASSIGNED) {
      assigned_.insert(task_pair);
      if (core_->raft_->IsLeader()) {
        LOG_TRACE();
        core_->task_mgr_->ProcessTask(entry.task());
      }
    } else if (move_to_task_status == TaskStatus::TASK_STATUS_COMPLETED) {
      completed_.insert(task_pair);
    } else {
      LOG_ERROR("Unknown move_to task status");
      return failure;
    }
  } else {
    LOG_ERROR("Unknown Entry's command");
    return failure;
  }

  return Status();
}

}  // namespace raft
}  // namespace abeille
