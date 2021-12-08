#include "worker_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"
#include "user/include/data_processor.hpp"

using abeille::user::ProcessData;
using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace worker {

void Client::CommandsHandler(const WorkerConnectResponse *resp) {
  switch (resp->command()) {
    case WORKER_COMMAND_NONE:
      handleCommandNone(resp);
      break;
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
      handleCommandUnrecognized(resp);
      break;
  }
}

void Client::handleCommandNone(const WorkerConnectResponse *response) {}

void Client::handleCommandAssign(const WorkerConnectResponse *response) {
  if (response->task_id() == 0) {
    LOG_ERROR("got assigned zero task");
  } else {
    LOG_INFO("got assigned [%llu] task", response->task_id());
    task_id_ = response->task_id();
    status_ = WORKER_STATUS_BUSY;
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
  ProcessData(task_data, task_result_);
  LOG_INFO("finished processing data");
  status_ = WORKER_STATUS_COMPLETED;
}

void Client::handleCommandUnrecognized(const WorkerConnectResponse *response) {
  LOG_ERROR("unrecognized user command");
}

}  // namespace worker
}  // namespace abeille
