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
  LOG_INFO("worker connection request from [%s]", address.c_str());

  WorkerConnectRequest request;
  auto &worker = workers_[worker_id];

  while (stream->Read(&request)) {
    // check if we are the leader
    if (!raft_consensus_->IsLeader()) {
      LOG_INFO("redirecting [%s] to the leader...", address.c_str());

      WorkerConnectResponse response;
      response.set_leader_id(raft_consensus_->LeaderID());
      response.set_command(WORKER_COMMAND_REDIRECT);

      if (!stream->Write(response)) {
        LOG_WARN("connection with [%s] was lost", address.c_str());
      }

      return Status::CANCELLED;
    }

    // update the status of the worker
    worker.status = request.status();

    // check if the worker has completed processing a data
    if (request.status() == WORKER_STATUS_COMPLETED) {
      LOG_DEBUG("[%s] has finished [%llu] task with result", address.c_str(),
                request.task_id());
    }

    WorkerConnectResponse response;
    response.set_leader_id(raft_consensus_->LeaderID());
    response.set_command(worker.command);

    if (worker.command == WORKER_COMMAND_PROCESS) {
      LOG_TRACE();
      if (worker.task && worker.task->has_task_data()) {
        response.set_task_id(worker.task->id());
        // TODO: get rid of reallocation
        auto task_data = new TaskData(worker.task->task_data());
        response.set_allocated_task_data(task_data);
      } else {
        LOG_ERROR("process command, but null task data");
      }
    }
    worker.command = WORKER_COMMAND_NONE;

    if (!stream->Write(response)) {
      break;
    }

    // LOG_DEBUG("[%s] is [%s]", address.c_str(),
    //           WorkerStatus_Name(request.status()).c_str());

    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  LOG_WARN("connection with worker [%s] was lost", address.c_str());

  return Status::OK;
}

Status WorkerServiceImpl::AssignTask(const AssignTaskRequest *request,
                                     AssignTaskResponse *response) {
  LOG_INFO("assigning task [%llu]", request->task_id());
  // auto it = std::find_if(workers_.begin(), workers_.end(),
  //                        [](const WorkerWrapper &worker) {
  //                          return worker.status == WORKER_STATUS_IDLE;
  //                        });

  auto it = workers_.begin();
  for (; it != workers_.end(); ++it) {
    if (it->second.status == WORKER_STATUS_IDLE) {
      it->second.status = WORKER_STATUS_BUSY;
      break;
    }
  }

  if (it == workers_.end()) {
    response->set_success(false);
  } else {
    response->set_success(true);
    response->set_worker_id(it->first);
    LOG_INFO("assigned [%llu] task to [%s]", request->task_id(),
             uint2address(it->first).c_str());
  }

  return Status::OK;
}

Status WorkerServiceImpl::SendTask(Task *task, SendTaskResponse *response) {
  // TODO: rename assignee to worker_id
  if (task) {
    uint64_t worker_id = task->worker_id();
    auto it = workers_.find(worker_id);
    if (it == workers_.end()) {
      LOG_ERROR("could not find worker [%s]", uint2address(worker_id).c_str());
      return Status::CANCELLED;
    }

    LOG_INFO("successfully sent [%llu] task to [%s]", task->id(),
             uint2address(worker_id).c_str());
    it->second.command = WORKER_COMMAND_PROCESS;
    it->second.task = task;
  } else {
    LOG_ERROR("send task, but null task");
  }

  return Status::OK;
}

Status WorkerServiceImpl::GetWorkerResult(const GetWorkerResultRequest *request,
                                          GetWorkerResultResponse *response) {
  uint64_t worker_id = request->worker_id();
  auto it = workers_.find(worker_id);
  if (it == workers_.end()) {
    LOG_ERROR("could not find worker with given id [%llu]", worker_id);
    return Status::CANCELLED;
  }

  if (it->second.task->id() != request->task_id()) {
    LOG_ERROR("request's and worker's task id mismatch");
    return Status::CANCELLED;
  }

  if (it->second.status == WORKER_STATUS_IDLE) {
    LOG_ERROR("worker [%llu] is idle", worker_id);
    return Status::CANCELLED;
  } else if (it->second.status == WORKER_STATUS_BUSY) {
    LOG_INFO("worker [%llu] is busy", worker_id);
    return Status::CANCELLED;
  }

  response->set_allocated_task_result(it->second.task->mutable_task_result());
  LOG_INFO("successfully returned worker result");

  return Status::OK;
}

}  // namespace raft_node
}  // namespace abeille
