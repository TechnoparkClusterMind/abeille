#include "task_manager.hpp"

#include "logger.hpp"

namespace abeille {
namespace raft_node {

error TaskManager::UploadData(TaskData *task_data,
                              UploadDataResponse *response) {
  // TODO: add raft logic - send entries

  // auto task = new Task();
  // task->set_allocated_task_data(task_data);
  // task->set_id(last_task_id_);

  // auto request = std::make_unique<AssignTaskRequest>();
  // request->set_task_id(last_task_id_);

  // auto resp = std::make_unique<AssignTaskResponse>();

  // auto status = core_->worker_service_->AssignTask(request.get(),
  // resp.get()); if (!status.ok()) {
  //   LOG_ERROR("failed to assign the task");
  // } else if (resp->success()) {
  //   LOG_DEBUG("successfully assigned the task");
  //   task->set_assignee(resp->worker_id());

  //   auto send_task_request = std::make_unique<SendTaskRequest>();
  //   send_task_request->set_allocated_task(task);

  //   auto send_task_response = std::make_unique<SendTaskResponse>();

  //   LOG_DEBUG("send the task...");
  //   core_->worker_service_->SendTask(send_task_request.get(),
  //                                    send_task_response.get());
  // } else {
  //   LOG_WARN("failed to assign the task (probably all workers are busy)");
  // }

  response->set_task_id(last_task_id_);
  ++last_task_id_;

  return error();
}

}  // namespace raft_node
}  // namespace abeille
