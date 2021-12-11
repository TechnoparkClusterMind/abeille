#include "raft/rpc/include/worker_service.hpp"

#include <iterator>
#include <thread>

#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

namespace abeille {
namespace raft {

void WorkerServiceImpl::ConnectHandler(uint64_t client_id) {
  client_ids_.push_back(client_id);
  auto &cw = client_wrappers_[client_id];
  cw.status = WORKER_STATUS_IDLE;
}

void WorkerServiceImpl::CommandHandler(uint64_t client_id, ConnResp *resp) {
  resp->set_leader_id(raft_consensus_->LeaderID());

  // check if we are the leader
  if (!raft_consensus_->IsLeader()) {
    LOG_INFO("redirecting [%s] to the leader...",
             uint2address(client_id).c_str());
    resp->set_command(WORKER_COMMAND_REDIRECT);
    return;
  }

  auto &cw = client_wrappers_[client_id];
  if (cw.commands.empty()) {
    resp->set_command(WORKER_COMMAND_NONE);
    return;
  }

  auto command = cw.commands.back();
  cw.commands.pop();

  resp->set_command(command);

  switch (command) {
    case WORKER_COMMAND_ASSIGN:
      handleCommandAssign(cw, resp);
      break;
    case WORKER_COMMAND_PROCESS:
      handleCommandProcess(cw, resp);
      break;
    default:
      break;
  }
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
  auto &cw = client_wrappers_[client_id];

  // update the status of the worker
  cw.status = req->status();

  switch (cw.status) {
    case WORKER_STATUS_COMPLETED:
      handleStatusCompleted(cw, req);
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

// TODO: refactor this crap
error WorkerServiceImpl::AssignTask(uint64_t task_id, uint64_t &worker_id) {
  LOG_DEBUG("assigning task#[%llu]", task_id);

  while (client_ids_.empty()) {
    LOG_WARN("no workers are connected, waiting...");
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  // round-robin load balancing
  size_t count = 0;
  auto &cw = client_wrappers_[client_ids_[curr_client_id_]];
  while (cw.status == WORKER_STATUS_DOWN) {
    ++count;
    if (count == client_ids_.size()) {
      count = 0;
      LOG_DEBUG("no active worker to assign the task, retrying...");
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    curr_client_id_ = (curr_client_id_ + 1) % client_ids_.size();
    cw = client_wrappers_[client_ids_[curr_client_id_]];
  }

  cw.task.set_id(task_id);
  cw.status = WORKER_STATUS_BUSY;
  cw.commands.push(WORKER_COMMAND_ASSIGN);

  worker_id = client_ids_[curr_client_id_];
  LOG_DEBUG("assigned task#[%llu] to [%s]", task_id,
            uint2address(worker_id).c_str());

  curr_client_id_ = (curr_client_id_ + 1) % client_ids_.size();

  return error();
}

error WorkerServiceImpl::ProcessTask(const Task &task) {
  uint64_t worker_id = task.worker_id();
  auto it = client_wrappers_.find(worker_id);
  if (it == client_wrappers_.end()) {
    return error("worker with given id was not found");
  }

  it->second.commands.push(WORKER_COMMAND_PROCESS);
  it->second.task = task;

  LOG_DEBUG("successfully sent [%llu] task to [%s]", task.id(),
            uint2address(worker_id).c_str());

  return error();
}

error WorkerServiceImpl::GetResult(uint64_t worker_id,
                                   TaskResult *task_result) {
  auto it = client_wrappers_.find(worker_id);
  if (it == client_wrappers_.end()) {
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

void WorkerServiceImpl::DisconnectHandler(uint64_t client_id) {
  auto it = client_wrappers_.find(client_id);
  if (it == client_wrappers_.end()) {
    LOG_ERROR("unregistered client was disconnected");
  }
  it->second.status = WORKER_STATUS_DOWN;
}

}  // namespace raft
}  // namespace abeille
