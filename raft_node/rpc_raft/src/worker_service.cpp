#include "worker_service.hpp"

#include <thread>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace raft_node {

void WorkerServiceImpl::CommandHandler(uint64_t client_id, ConnResp *resp) {
  resp->set_leader_id(raft_consensus_->LeaderID());

  // check if we are the leader
  if (!raft_consensus_->IsLeader()) {
    LOG_INFO("redirecting [%s] to the leader...",
             uint2address(client_id).c_str());
    resp->set_command(WORKER_COMMAND_REDIRECT);
    return;
  }

  auto &cw = clients_[client_id];
  resp->set_command(cw.command);

  switch (cw.command) {
    case WORKER_COMMAND_ASSIGN:
      handleCommandAssign(cw, resp);
      break;
    case WORKER_COMMAND_PROCESS:
      handleCommandProcess(cw, resp);
      break;
    default:
      break;
  }

  // reset command so to execute a command only once
  cw.command = WORKER_COMMAND_NONE;
}

void WorkerServiceImpl::handleCommandAssign(ClientWrapper &cw, ConnResp *resp) {
  resp->set_task_id(cw.task.id());
}

void WorkerServiceImpl::handleCommandProcess(ClientWrapper &cw,
                                             ConnResp *resp) {
  if (cw.task.has_task_data()) {
    resp->set_task_id(cw.task.id());
    // TODO: get rid of reallocation
    auto task_data = new TaskData(cw.task.task_data());
    resp->set_allocated_task_data(task_data);
  } else {
    LOG_ERROR("process command, but null task data");
  }
}

void WorkerServiceImpl::StatusHandler(uint64_t client_id, const ConnReq *req) {
  auto &worker = clients_[client_id];

  // update the status of the worker
  worker.status = req->status();

  switch (worker.status) {
    case WORKER_STATUS_COMPLETED:
      handleStatusCompleted(worker, req);
      break;
    default:
      break;
  }
}

void WorkerServiceImpl::handleStatusCompleted(ClientWrapper &cw,
                                              const ConnReq *req) {
  LOG_DEBUG("worker has finished task#[%llu]", req->task_id());
  // TODO: implement me
}

error WorkerServiceImpl::AssignTask(uint64_t task_id, uint64_t &worker_id) {
  LOG_DEBUG("assigning task#[%llu]", task_id);

  auto it = clients_.begin();
  for (; it != clients_.end(); ++it) {
    if (it->second.status == WORKER_STATUS_IDLE) {
      it->second.task.set_id(task_id);
      it->second.status = WORKER_STATUS_BUSY;
      it->second.command = WORKER_COMMAND_ASSIGN;
      break;
    }
  }

  if (it == clients_.end()) {
    return error("no idle workers were found");
  }

  worker_id = it->first;
  LOG_DEBUG("assigned task#[%llu] to [%s]", task_id,
            uint2address(it->first).c_str());

  return error();
}

error WorkerServiceImpl::SendTask(const Task &task) {
  uint64_t worker_id = task.worker_id();
  auto it = clients_.find(worker_id);
  if (it == clients_.end()) {
    return error("worker with given id was not found");
  }

  it->second.command = WORKER_COMMAND_PROCESS;
  it->second.task = task;

  LOG_DEBUG("successfully sent [%llu] task to [%s]", task.id(),
            uint2address(worker_id).c_str());

  return error();
}

error WorkerServiceImpl::GetWorkerResult(uint64_t worker_id,
                                         TaskResult *task_result) {
  auto it = clients_.find(worker_id);
  if (it == clients_.end()) {
    return error("worker with given id was not found");
  }

  if (it->second.status == WORKER_STATUS_IDLE) {
    return error("worker is idle");
  } else if (it->second.status == WORKER_STATUS_BUSY) {
    return error("worker is busy");
  }

  task_result = it->second.task.mutable_task_result();
  LOG_DEBUG("successfully returned worker result");

  return error();
}

}  // namespace raft_node
}  // namespace abeille
