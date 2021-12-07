#include "task_manager.hpp"

#include "logger.hpp"

namespace abeille {
namespace raft_node {

error TaskManager::UploadData(TaskData *task_data, UploadDataResponse *response) {
  response->set_task_id(last_task_id_);
  ++last_task_id_;

  Entry entry;

  auto task = new Task();
  task->set_allocated_task_data(task_data);

  entry.set_allocated_task(task);

  auto request = std::make_unique<AssignTaskRequest>();
  request->set_task_id(response->task_id());

  auto resp = std::make_unique<AssignTaskResponse>();

  auto status = core_->worker_service_->AssignTask(request.get(), resp.get());
  if (!status.ok()) {
    LOG_ERROR("failed to assign the task");
  }

  return error();
}

}  // namespace raft_node
}  // namespace abeille
