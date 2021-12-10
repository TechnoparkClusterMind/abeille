#include "worker_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"
#include "user/include/data_processor.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace worker {

void Client::CommandHandler(const ConnResp &resp) {
  switch (resp.command()) {
    case WORKER_COMMAND_REDIRECT:
      handleCommandRedirect(resp);
      break;
    case WORKER_COMMAND_ASSIGN:
      handleCommandAssign(resp);
      break;
    case WORKER_COMMAND_PROCESS:
      handleCommandProcess(resp);
      break;
    default:
      break;
  }
}

void Client::handleCommandAssign(const ConnResp &resp) {
  if (resp.task_id() == 0) {
    LOG_ERROR("got assigned zero task");
  } else {
    task_id_ = resp.task_id();
    status_ = WORKER_STATUS_BUSY;
    LOG_INFO("got assigned [%llu] task", resp.task_id());
  }
}

void Client::handleCommandProcess(const ConnResp &resp) {
  if (!resp.has_task_data()) {
    LOG_ERROR("got asked to process null data");
  } else {
    std::thread(&Client::processData, this, resp.task_data()).detach();
  }
}

void Client::handleCommandRedirect(const ConnResp &resp) {
  connected_ = false;
  leader_id_ = resp.leader_id();
  address_ = uint2address(resp.leader_id());
  LOG_INFO("got redirected to the [%s]", address_.c_str());
  connect();
}

void Client::processData(const TaskData &task_data) {
  LOG_INFO("processing data...");
  task_result_ = new TaskResult();
  abeille::user::ProcessData(task_data, task_result_);
  LOG_INFO("finished processing data");
  status_ = WORKER_STATUS_COMPLETED;
}

void Client::StatusHandler(ConnReq &req) {
  req.set_status(status_);
  switch (status_) {
    case WORKER_STATUS_COMPLETED:
      handleStatusCompleted(req);
      break;
    default:
      break;
  }
}

void Client::handleStatusCompleted(ConnReq &req) {
  status_ = WORKER_STATUS_IDLE;
  req.set_task_id(task_id_);
  req.set_allocated_task_result(task_result_);
}

}  // namespace worker
}  // namespace abeille
