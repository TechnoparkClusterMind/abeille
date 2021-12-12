#include "worker/include/worker_client.hpp"

#include "user/include/data_processor.hpp"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

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
  if (resp.task_id().client_id() == 0) {
    LOG_ERROR("client id is zero");
    return;
  }

  task_id_ = resp.task_id();
  status_ = WORKER_STATUS_BUSY;
  LOG_INFO("got assigned [%llu] task", resp.task_id());
}

void Client::handleCommandProcess(const ConnResp &resp) {
  if (resp.task_data().empty()) {
    LOG_ERROR("empty task data");
  } else {
    std::thread(&Client::processTaskData, this, resp.task_data()).detach();
  }
}

void Client::handleCommandRedirect(const ConnResp &resp) {
  connected_ = false;
  leader_id_ = resp.leader_id();
  address_ = uint2address(resp.leader_id());
  LOG_INFO("got redirected to the [%s]", address_.c_str());
  connect();
}

void Client::processTaskData(const Bytes &task_data) {
  LOG_INFO("processing data...");

  Task::Data td;
  td.ParseFromString(task_data);

  Task::Result ts;
  abeille::user::ProcessData(td, ts);
  task_result_ = ts.SerializeAsString();

  status_ = WORKER_STATUS_COMPLETED;
  LOG_INFO("finished processing data");
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
  status_ = WORKER_STATUS_IDLE;
}

void Client::handleStatusCompleted(ConnReq &req) {
  auto task_state = new TaskState;
  LOG_DEBUG("[%s], [%s]", task_id_.filename().c_str(), uint2address(task_id_.client_id()).c_str());
  task_state->set_allocated_task_id(new TaskID(task_id_));
  task_state->set_task_result(task_result_);
  req.set_allocated_task_state(task_state);
}

}  // namespace worker
}  // namespace abeille
