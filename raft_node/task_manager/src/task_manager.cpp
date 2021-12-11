#include "raft_node/task_manager/include/task_manager.hpp"

#include <memory>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

error TaskManager::UploadData(const TaskData &task_data, uint64_t &task_id) {
  auto task = new Task();
  task->set_allocated_task_data(new TaskData(task_data));
  task->set_id(last_task_id_);

  Entry entry;
  entry.set_command(RAFT_COMMAND_ADD);

  uint64_t worker_id = 0;
  auto svc = static_cast<WorkerServiceImpl *>(core_->worker_service_.get());
  error err = svc->AssignTask(last_task_id_, worker_id);

  auto add_request = new AddRequest();
  if (!err.ok()) {
    LOG_DEBUG(err.what().c_str());
    // add_request->set_to(TASK_STATUS_UNASSIGNED);
  } else {
    LOG_INFO("successfully assigned task#[%llu] to [%s]", last_task_id_,
             uint2address(worker_id).c_str());
    task->set_worker_id(worker_id);
    add_request->set_to(TASK_STATUS_ASSIGNED);
  }

  entry.set_allocated_add_request(add_request);
  entry.set_allocated_task(task);
  entry.set_term(core_->raft_->GetTerm());

  task_id = last_task_id_;
  ++last_task_id_;

  core_->raft_pool_->AppendAll(entry);

  return error();
}

error TaskManager::ProcessTask(const Task &task) {
  auto svc = static_cast<WorkerServiceImpl *>(core_->worker_service_.get());
  error err = svc->ProcessTask(task);
  return err;
}

}  // namespace raft_node
}  // namespace abeille
