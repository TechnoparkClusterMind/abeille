#include "user/include/user_client.hpp"

#include <chrono>
#include <vector>

#include "user/include/registry.hpp"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace user {

void Client::CommandHandler(const ConnResp &resp) {
  switch (resp.command()) {
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

error Client::UploadData(const Task::Data &task_data) {
  status_ = USER_STATUS_UPLOAD_DATA;
  Registry::Instance().task_datas.push(task_data);
  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [&] { return status_ == USER_STATUS_IDLE; });
  return error();
}

void Client::handleCommandRedirect(const ConnResp &resp) {
  connected_ = false;
  leader_id_ = resp.leader_id();
  address_ = uint2address(resp.leader_id());
  LOG_INFO("got redirected to the [%s]", address_.c_str());
  connect();
}

void Client::handleCommandAssign(const ConnResp &resp) {
  LOG_DEBUG("task number [%llu]", resp.task_state().task_id().number());
  LOG_DEBUG("your id [%llu]", resp.task_state().task_id().client_id());
  // TODO: implement me
}

void Client::handleCommandResult(const ConnResp &resp) {
  // TODO: implement me
}

void Client::StatusHandler(ConnReq &req) {
  {
    std::lock_guard<std::mutex> lk(mutex_);
    switch (status_) {
      case USER_STATUS_UPLOAD_DATA:
        handleStatusUploadData(req);
        break;
      default:
        break;
    }
  }

  req.set_status(status_);
  status_ = USER_STATUS_IDLE;

  cv_.notify_one();
}

void Client::handleStatusUploadData(ConnReq &req) {
  if (Registry::Instance().task_datas.empty()) {
    LOG_ERROR("empty task data queue");
  } else {
    Registry::Instance().task_datas.front().SerializeToString(req.mutable_task_data());
    Registry::Instance().task_datas.pop();
  }
}

}  // namespace user
}  // namespace abeille
