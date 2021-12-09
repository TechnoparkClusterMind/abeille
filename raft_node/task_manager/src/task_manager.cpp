#include "task_manager.hpp"

#include <memory>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

error TaskManager::UploadData(TaskData *task_data,
                              UploadDataResponse *response) {
  // TODO: add raft logic - send entries

  auto task = new Task();
  task->set_allocated_task_data(task_data);
  task->set_id(last_task_id_);

  auto svc = static_cast<WorkerServiceImpl *>(core_->worker_service_.get());

  auto request = std::make_unique<AssignTaskRequest>();
  request->set_task_id(last_task_id_);
  auto resp = std::make_unique<AssignTaskResponse>();

  auto status = svc->AssignTask(request.get(), resp.get());

  Entry entry;
  entry.set_command(RAFT_COMMAND_ADD);

  auto add_request = new AddRequest();
  if (!status.ok()) {
    LOG_ERROR("assign task request failed");
    add_request->set_to(TASK_STATUS_UNASSIGNED);
  } else if (resp->success()) {
    LOG_INFO("successfully assigned task#[%llu] to [%s]", last_task_id_,
             uint2address(resp->worker_id()).c_str());
    task->set_worker_id(resp->worker_id());
    add_request->set_to(TASK_STATUS_ASSIGNED);
  } else {
    LOG_WARN("failed to assign the task (probably all workers are busy)");
    add_request->set_to(TASK_STATUS_UNASSIGNED);
  }
  entry.set_allocated_add_request(add_request);

  entry.set_allocated_task(task);
  ++last_task_id_;

  core_->raft_pool_->AppendAll(entry);

  return error();
}

}  // namespace raft_node
}  // namespace abeille
