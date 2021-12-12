#include "raft/task_manager/include/task_manager.hpp"

#include <memory>

#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

namespace abeille {
namespace raft {

error TaskManager::UploadTaskData(const Bytes &task_data, const TaskID &task_id) {
  auto task_wrapper = new TaskWrapper();
  task_wrapper->set_task_data(task_data);
  task_wrapper->set_allocated_task_id(new TaskID(task_id));

  Entry entry;
  entry.set_command(RAFT_COMMAND_ADD);

  uint64_t worker_id = 0;
  auto svc = static_cast<WorkerServiceImpl *>(core_->worker_service_.get());
  error err = svc->AssignTask(task_id, worker_id);
  if (!err.ok()) {
    return err.what();
  }

  auto add_request = new AddRequest();
  LOG_DEBUG("assigned task#[%s] to [%s]", task_id.filename().c_str(), uint2address(worker_id).c_str());
  task_wrapper->set_worker_id(worker_id);
  add_request->set_to(TASK_STATUS_ASSIGNED);

  entry.set_allocated_add_request(add_request);
  entry.set_allocated_task_wrapper(task_wrapper);
  entry.set_term(core_->raft_->GetTerm());

  core_->raft_pool_->AppendAll(entry);

  return error();
}

error TaskManager::ProcessTask(const TaskWrapper &task_wrapper) {
  auto svc = static_cast<WorkerServiceImpl *>(core_->worker_service_.get());
  error err = svc->ProcessTask(task_wrapper);
  return err;
}

error TaskManager::UploadTaskResult(const TaskID &task_id, const Bytes &task_result) {
  LOG_DEBUG("uploading task result...");

  Entry entry;
  entry.set_command(RAFT_COMMAND_MOVE);

  auto move_request = new MoveRequest();
  move_request->set_from(TASK_STATUS_ASSIGNED);
  move_request->set_to(TASK_STATUS_COMPLETED);
  entry.set_allocated_move_request(move_request);

  auto task_wrapper = new TaskWrapper();
  task_wrapper->set_allocated_task_id(new TaskID(task_id));
  task_wrapper->set_task_result(task_result);
  entry.set_allocated_task_wrapper(task_wrapper);

  entry.set_term(core_->raft_->GetTerm());
  core_->raft_pool_->AppendAll(entry);

  return error();
}

error TaskManager::SendTaskResult(const TaskWrapper &task_wrapper) {
  auto svc = static_cast<UserServiceImpl *>(core_->user_service_.get());
  error err = svc->SendTaskResult(task_wrapper);
  return err;
}

}  // namespace raft
}  // namespace abeille
