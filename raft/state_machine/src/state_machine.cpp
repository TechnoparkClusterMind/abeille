#include "raft/state_machine/include/state_machine.hpp"

#include "utils/include/logger.hpp"

#define ASSIGNED TaskStatus::TASK_STATUS_ASSIGNED;
#define COMPLETED TaskStatus::TASK_STATUS_COMPLETED;

namespace abeille {
namespace raft {

void StateMachine::Commit(Index index) noexcept {
  // log will be refactored to singleton
  LOG_TRACE();
  Index last_log_index = core_->raft_->log_->LastIndex();
  Index max_idx =  last_log_index >= index ? index : last_log_index;

  for (Index idx = commit_index_ + 1; idx <= max_idx; ++idx) {
    auto status = applyCommand((*core_->raft_->log_)[idx]);
    if (!status.ok()) {
      LOG_ERROR("Error while applying commited entry");
    }
  }

  commit_index_ = max_idx;
  // LOG_DEBUG("StateMachine: %lu - %lu", assigned_.size(), completed_.size());
}

error StateMachine::applyCommand(const Log::EntryConstReference entry) noexcept {
  auto tw = TaskIDWrapper("duck", entry.task_wrapper().task_id().client_id());
  auto task_pair = std::make_pair(tw, entry.task_wrapper());
  auto failure = error(error::Code::FAILURE);

  if (entry.command() == RaftCommand::RAFT_COMMAND_ADD) {
    auto add_task_status = entry.add_request().to();
    if (add_task_status == TaskStatus::TASK_STATUS_ASSIGNED) {
      assigned_.insert(task_pair);
      if (core_->raft_->IsLeader()) {
        core_->task_mgr_->ProcessTask(entry.task_wrapper());
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
      assigned_.erase(tw);
    } else if (move_from_task_status == TaskStatus::TASK_STATUS_COMPLETED) {
      LOG_WARN("Deleting completed task from state machine");
      completed_.erase(tw);
    } else {
      LOG_ERROR("Unknown move_from task status");
      return failure;
    }

    if (move_to_task_status == TaskStatus::TASK_STATUS_ASSIGNED) {
      assigned_.insert(task_pair);
      if (core_->raft_->IsLeader()) {
        core_->task_mgr_->ProcessTask(entry.task_wrapper());
      }
    } else if (move_to_task_status == TaskStatus::TASK_STATUS_COMPLETED) {
      completed_.insert(task_pair);
      if (core_->raft_->IsLeader()) {
        LOG_TRACE();
        core_->task_mgr_->SendTaskResult(entry.task_wrapper());
      }
    } else {
      LOG_ERROR("Unknown move_to task status");
      return failure;
    }
  } else {
    LOG_ERROR("Unknown Entry's command");
    return failure;
  }

  return error();
}

}  // namespace raft
}  // namespace abeille
