
#include "raft/rpc/include/user_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft {

void UserServiceImpl::ConnectHandler(uint64_t client_id) {
  LOG_INFO("user connection from [%s]", uint2address(client_id).c_str());
  auto &cw = client_wrappers_[client_id];
  cw.client_id = client_id;
}

void UserServiceImpl::CommandHandler(uint64_t client_id, ConnResp &resp) {
  resp.set_leader_id(raft_consensus_->LeaderID());

  // check if we are the leader
  if (!raft_consensus_->IsLeader()) {
    LOG_INFO("redirecting [%s] to the leader...", uint2address(client_id).c_str());
    resp.set_command(USER_COMMAND_REDIRECT);
    return;
  }

  auto &cw = client_wrappers_[client_id];
  if (cw.commands.empty()) {
    resp.set_command(USER_COMMAND_NONE);
    return;
  }

  auto command = cw.commands.front();
  cw.commands.pop();

  error err;
  switch (command) {
    case USER_COMMAND_ASSIGN:
      err = handleCommandAssign(cw, resp);
    case USER_COMMAND_RESULT:
    default:
      break;
  }

  if (err.ok()) {
    resp.set_command(command);
  } else {
    resp.set_command(USER_COMMAND_NONE);
  }
}

error UserServiceImpl::handleCommandAssign(ClientWrapper &cw, ConnResp &resp) {
  if (cw.task_state_queue.empty()) {
    return error("empty task state queue");
  }

  auto task_state = new TaskState(cw.task_state_queue.front());
  task_state->mutable_task_id()->set_client_id(cw.client_id);
  resp.set_allocated_task_state(task_state);
  cw.task_state_queue.pop();

  return error();
}

error UserServiceImpl::handleCommandResult(ClientWrapper &cw, ConnResp &resp) {
  if (cw.task_state_queue.empty()) {
    return error("empty task state queue");
  }

  resp.set_allocated_task_state(new TaskState(cw.task_state_queue.front()));
  cw.task_state_queue.pop();

  return error();
}

void UserServiceImpl::StatusHandler(uint64_t client_id, const ConnReq &req) {
  auto &cw = client_wrappers_[client_id];

  // update the status of the client
  cw.status = req.status();

  error err;
  switch (cw.status) {
    case USER_STATUS_UPLOAD_DATA:
      err = handleStatusUploadData(cw, req);
      break;
    default:
      break;
  }

  if (!err.ok()) {
    LOG_ERROR("%s", err.what().c_str());
  }
}

error UserServiceImpl::handleStatusUploadData(ClientWrapper &cw, const ConnReq &req) {
  if (req.task_data().empty()) {
    return error("empty task data");
  }

  TaskState task_state;
  task_state.mutable_task_id()->set_client_id(cw.client_id);
  task_state.mutable_task_id()->set_filename(req.filename());

  error err = task_mgr_->UploadTaskData(req.task_data(), task_state.task_id());
  if (!err.ok()) {
    return err;
  }

  cw.task_state_queue.push(std::move(task_state));
  cw.commands.push(USER_COMMAND_ASSIGN);

  return error();
}

void UserServiceImpl::DisconnectHandler(uint64_t client_id) {
  auto it = client_wrappers_.find(client_id);
  if (it == client_wrappers_.end()) {
    LOG_ERROR("unregistered client was disconnected");
  }
  it->second.status = USER_STATUS_DOWN;
}

error UserServiceImpl::SendTaskResult(const TaskWrapper &task_wrapper) {
  auto client_id = task_wrapper.task_id().client_id();
  auto it = client_wrappers_.find(client_id);
  if (it == client_wrappers_.end()) {
    return error("unknown client id");
  }

  TaskState task_state;
  task_state.set_task_result(task_wrapper.task_result());

  it->second.task_state_queue.push(std::move(task_state));
  it->second.commands.push(USER_COMMAND_RESULT);

  return error();
}

}  // namespace raft
}  // namespace abeille
