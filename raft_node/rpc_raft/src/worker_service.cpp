#include "worker_service.hpp"

#include <thread>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

Status WorkerServiceImpl::Connect(ServerContext *context,
                                  ConnectStream *stream) {
  std::string address = ExtractAddress(context->peer());
  uint64_t worker_id = address2uint(address);
  LOG_INFO("connection request from [%s]", address.c_str());

  WorkerStatus request;
  WorkerState &worker = workers_[worker_id];

  while (stream->Read(&request)) {
    // check if we are the leader
    if (!isLeader()) {
      LOG_INFO("redirecting [%s] to the leader...", address.c_str());

      ConnectResponse response;
      response.set_leader_id(leader_id_);
      response.set_command(WorkerCommand::REDIRECT);

      if (!stream->Write(response)) {
        LOG_WARN("connection with [%s] was lost", address.c_str());
      }

      return Status::CANCELLED;
    }

    // update the status of the worker
    worker.status = request.status();

    // check if the worker has completed processing a data
    if (request.status() == NodeStatus::COMPLETED) {
      LOG_DEBUG("[%s] has finished [%llu] task with result", address.c_str(),
                request.task_id());
    }

    ConnectResponse response;
    response.set_leader_id(leader_id_);
    response.set_command(worker_command_);
    response.set_task_id(worker.task.id());

    if (worker_command_ == WorkerCommand::PROCESS) {
      if (worker.task.has_task_data()) {
        response.set_command(WorkerCommand::PROCESS);

        auto task_data = new TaskData(worker.task.task_data());
        response.set_allocated_task_data(task_data);
      } else {
        LOG_ERROR("process command, but null task data");
      }
    }
    worker_command_ = WorkerCommand::NONE;

    if (!stream->Write(response)) {
      break;
    }

    LOG_DEBUG("[%s] is [%s]", address.c_str(),
              NodeStatus_Name(request.status()).c_str());

    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  LOG_WARN("connection with [%s] was lost", address.c_str());

  return Status::OK;
}

Status WorkerServiceImpl::AssignTask(const AssignTaskRequest *request,
                                     AssignTaskResponse *response) {
  LOG_INFO("assigning task [%llu]", request->task_id());
  uint64_t worker_id = 0;
  for (auto it = workers_.begin(); it != workers_.end(); ++it) {
    if (it->second.status == NodeStatus::IDLE) {
      worker_id = it->first;
      it->second.status = NodeStatus::BUSY;
      it->second.task.set_id(request->task_id());
      break;
    }
  }

  if (worker_id == 0) {
    response->set_success(false);
  } else {
    worker_command_ = WorkerCommand::ASSIGN;
    response->set_success(true);
    response->set_worker_id(worker_id);
    LOG_INFO("assigned [%llu] task to [%llu]", request->task_id(), worker_id);
  }

  return Status::OK;
}

Status WorkerServiceImpl::SendTask(const SendTaskRequest *request,
                                   SendTaskResponse *response) {
  uint64_t worker_id = request->task().assignee();
  auto state = workers_.find(worker_id);
  if (state == workers_.end()) {
    LOG_ERROR("could not find worker with given id [%llu]", worker_id);
    return Status::CANCELLED;
  }

  if (state->second.task.id() == request->task().id()) {
    worker_command_ = WorkerCommand::PROCESS;
    LOG_INFO("successfully sent [%llu] task to [%llu]", request->task().id(),
             worker_id);
    state->second.task = request->task();
  } else {
    LOG_ERROR("task is sent to a wrong worker");
    return Status::CANCELLED;
  }
  return Status::OK;
}

Status WorkerServiceImpl::GetWorkerResult(const GetWorkerResultRequest *request,
                                          GetWorkerResultResponse *response) {
  uint64_t worker_id = request->worker_id();
  auto state = workers_.find(worker_id);
  if (state == workers_.end()) {
    LOG_ERROR("could not find worker with given id [%luu]", worker_id);
    return Status::CANCELLED;
  }

  if (state->second.task.id() != request->task_id()) {
    LOG_ERROR("request's and worker's task id mismatch");
    return Status::CANCELLED;
  }

  if (state->second.status == NodeStatus::IDLE) {
    LOG_ERROR("worker [%llu] is idle", worker_id);
    return Status::CANCELLED;
  } else if (state->second.status == NodeStatus::BUSY) {
    LOG_INFO("worker [%llu] is busy", worker_id);
    return Status::CANCELLED;
  }

  // TODO: move data converison to user-defined utils
  TaskResult *task_result = new TaskResult();
  task_result->set_result(state->second.task.task_result().result());
  response->set_allocated_task_result(task_result);

  return Status::OK;
}

}  // namespace raft_node
}  // namespace abeille
