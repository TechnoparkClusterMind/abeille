#include "user/include/user_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace user {

void Client::CommandHandler(const UserConnectResponse *resp) {
  switch (resp->command()) {
    case USER_COMMAND_REDIRECT:
      handleCommandRedirect(resp);
      break;
    case USER_COMMAND_ASSIGN:
      handleCommandAssign(resp);
      break;
    case USER_COMMAND_RESULT:
      handleCommandResult(resp);
      break;
    default:
      break;
  }
}

void Client::UploadData(TaskData *task_data) {
  status_ = USER_STATUS_UPLOAD_DATA;
  task_datas_.push(task_data);
}

void Client::handleCommandRedirect(const UserConnectResponse *response) {
  connected_ = false;
  leader_id_ = response->leader_id();
  address_ = uint2address(response->leader_id());
  LOG_INFO("got redirected to the [%s]", address_.c_str());
  connect();
}

void Client::handleCommandAssign(const ConnResp *resp) {
  LOG_DEBUG("task was given id [%llu]", resp->task_id());
  // TODO: implement me
}

void Client::handleCommandResult(const UserConnectResponse *response) {
  // TODO: implement me
}

void Client::StatusHandler(ConnReq *req) {
  req->set_status(status_);
  switch (status_) {
    case USER_STATUS_UPLOAD_DATA:
      handleStatusUploadData(req);
      break;
    default:
      break;
  }
}

void Client::handleStatusUploadData(ConnReq *req) {
  if (task_datas_.empty()) {
    LOG_ERROR("user status upload data, but empty task data queue");
  } else {
    req->set_allocated_task_data(task_datas_.front());
    task_datas_.pop();
  }
  status_ = USER_STATUS_NONE;
}

}  // namespace user
}  // namespace abeille
