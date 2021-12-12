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

Client::Client(const std::vector<std::string> &cluster_addresses) : UserClient(cluster_addresses) {}

void Client::CommandHandler(const ConnResp &resp) {
  error err;
  switch (resp.command()) {
    case USER_COMMAND_REDIRECT:
      err = handleCommandRedirect(resp);
      break;
    case USER_COMMAND_ASSIGN:
      err = handleCommandAssign(resp);
      break;
    case USER_COMMAND_RESULT:
      err = handleCommandResult(resp);
      break;
    default:
      break;
  }

  if (!err.ok()) {
    LOG_ERROR("%s", err.what().c_str());
  }
}

error Client::UploadData(const Task::Data &task_data, const std::string &filename) {
  status_ = USER_STATUS_UPLOAD_DATA;
  Registry::Instance().filenames.push(filename);
  Registry::Instance().task_datas.push(task_data);
  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [&] { return status_ == USER_STATUS_IDLE; });
  return error();
}

error Client::handleCommandRedirect(const ConnResp &resp) {
  connected_ = false;
  leader_id_ = resp.leader_id();
  address_ = uint2address(resp.leader_id());
  LOG_INFO("got redirected to the [%s]", address_.c_str());
  connect();
  return error();
}

error Client::handleCommandAssign(const ConnResp &resp) {
  LOG_DEBUG("[%s] was assigned", resp.task_state().task_id().filename().c_str());
  LOG_DEBUG("your id [%llu]", resp.task_state().task_id().client_id());
  // TODO: implement me
  return error();
}

error Client::handleCommandResult(const ConnResp &resp) {
  Task::Result task_result;
  task_result.ParseFromString(resp.task_state().task_result());
  LOG_DEBUG("recieved result = [%d]", task_result.result());
  return error();
}

void Client::StatusHandler(ConnReq &req) {
  {
    std::lock_guard<std::mutex> lk(mutex_);

    error err;
    switch (status_) {
      case USER_STATUS_UPLOAD_DATA:
        err = handleStatusUploadData(req);
        break;
      default:
        break;
    }

    if (err.ok()) {
      req.set_status(status_);
    } else {
      req.set_status(USER_STATUS_IDLE);
    }

    status_ = USER_STATUS_IDLE;
  }

  cv_.notify_one();
}

error Client::handleStatusUploadData(ConnReq &req) {
  if (Registry::Instance().task_datas.empty()) {
    return error("empty task data queue");
  }

  if (Registry::Instance().filenames.empty()) {
    return error("empty filenames queue");
  }

  req.set_filename(Registry::Instance().filenames.front());
  Registry::Instance().filenames.front();

  Registry::Instance().task_datas.front().SerializeToString(req.mutable_task_data());
  Registry::Instance().task_datas.pop();

  return error();
}

}  // namespace user
}  // namespace abeille
