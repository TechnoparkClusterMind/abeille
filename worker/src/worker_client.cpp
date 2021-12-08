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

void Client::CommandHandler(const WorkerConnectResponse *resp) {
  switch (resp->command()) {
    case WORKER_COMMAND_REDIRECT:
      handleCommandRedirect(resp);
      break;
    case WORKER_COMMAND_PROCESS:
      handleCommandProcess(resp);
      break;
    default:
      break;
  }
}

void Client::handleCommandProcess(const WorkerConnectResponse *response) {
  if (!response->has_task_data()) {
    LOG_ERROR("got asked to process null data");
  } else {
    std::thread(&Client::processData, this, response->task_data()).detach();
  }
}

void Client::handleCommandRedirect(const WorkerConnectResponse *response) {
  connected_ = false;
  leader_id_ = response->leader_id();
  address_ = uint2address(response->leader_id());
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

void Client::StatusHandler(ConnReq *req) {
  req->set_status(status_);
  switch (status_) {
    case WORKER_STATUS_COMPLETED:
      handleStatusCompleted(req);
      break;
    default:
      break;
  }
}

void Client::handleStatusCompleted(ConnReq *req) {
  status_ = WORKER_STATUS_IDLE;
  req->set_task_id(task_id_);
  req->set_allocated_task_result(task_result_);
}

}  // namespace worker
}  // namespace abeille
