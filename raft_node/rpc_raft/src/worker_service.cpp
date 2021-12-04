#include "worker_service.hpp"

#include <thread>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

Status WorkerServiceImpl::Connect(ServerContext *context, ConnectStream *stream) {
  std::string address = ExtractAddress(context->peer());
  LOG_INFO("connection request from [%s]", address.c_str());

  ConnectResponse response;
  if (!IsLeader()) {
    LOG_INFO("redirecting to the leader...");
    response.set_leader_id(leader_id_);
    return Status::CANCELLED;
  }

  WorkerStatus request;
  bool ok = stream->Read(&request);
  if (!ok) {
    LOG_ERROR("failed to read from the stream with [%s]", address.c_str());
    return Status::CANCELLED;
  }

  uint64_t id = address2uint(address);
  while (true) {
    ok = stream->Write(response);
    if (!ok) {
      LOG_WARN("connection with [%s] was lost", address.c_str());
      break;
    }

    ok = stream->Read(&request);
    if (!ok) {
      LOG_WARN("connection with [%s] was lost", address.c_str());
      break;
    }

    workers_[id].status = request.status();
    if (request.has_task_result()) {
      // TODO: move data converison to user-defined utils
      TaskResult *task_result = new TaskResult();
      task_result->set_result(request.task_result().result());
      workers_[id].task.set_allocated_task_result(task_result);
    }

    LOG_DEBUG("[%s] is [%d]", address.c_str(), request.status());

    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  return Status::OK;
}

Status WorkerServiceImpl::AssignTask(ServerContext *context, const AssignTaskRequest *request,
                                     AssignTaskResponse *response) {
  LOG_DEBUG("assigning task [%llu]", request->task_id());
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
    response->set_success(true);
    response->set_worker_id(worker_id);
    LOG_INFO("assigned [%llu] task to [%llu]", request->task_id(), worker_id);
  }

  return Status::OK;
}

Status WorkerServiceImpl::SendTask(ServerContext *context, const SendTaskRequest *request, SendTaskResponse *response) {
  uint64_t worker_id = request->task().assignee();
  auto state = workers_.find(worker_id);
  if (state == workers_.end()) {
    LOG_ERROR("could not find worker with given id [%luu]", worker_id);
    return Status::CANCELLED;
  }

  if (state->second.task.id() == request->task().id()) {
    LOG_INFO("successfully sent [%llu] task to [%llu]", request->task().id(), worker_id);
    state->second.task = request->task();
  } else {
    LOG_ERROR("task is sent to a wrong worker");
    return Status::CANCELLED;
  }
  return Status::OK;
}

Status WorkerServiceImpl::GetWorkerResult(ServerContext *context, const GetWorkerResultRequest *request,
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
