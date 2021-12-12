
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
  auto &cw = client_wrappers_[client_id];
  cw.task_state.set_allocated_task_id(new TaskID());
  cw.task_state.mutable_task_id()->set_client_id(client_id);
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

  resp.set_command(command);

  switch (command) {
    case USER_COMMAND_ASSIGN:
      handleCommandAssign(cw, resp);
    case USER_COMMAND_RESULT:
    default:
      break;
  }
}

void UserServiceImpl::handleCommandAssign(ClientWrapper &cw, ConnResp &resp) {
  resp.set_allocated_task_state(new TaskState(cw.task_state));
}

void UserServiceImpl::StatusHandler(uint64_t client_id, const ConnReq &req) {
  auto &cw = client_wrappers_[client_id];

  // update the status of the client
  cw.status = req.status();

  switch (cw.status) {
    case USER_STATUS_UPLOAD_DATA:
      handleStatusUploadData(cw, req);
      break;
    default:
      break;
  }
}

void UserServiceImpl::handleStatusUploadData(ClientWrapper &cw, const ConnReq &req) {
  if (req.task_data().empty()) {
    LOG_ERROR("empty task data");
  } else {
    cw.task_state.mutable_task_id()->set_number(cw.task_number++);
    error err = task_mgr_->UploadTaskData(req.task_data(), cw.task_state.task_id());
    if (!err.ok()) {
      LOG_ERROR("%s", err.what().c_str());
      return;
    }
    cw.commands.push(USER_COMMAND_ASSIGN);
  }
}

void UserServiceImpl::DisconnectHandler(uint64_t client_id) {
  auto it = client_wrappers_.find(client_id);
  if (it == client_wrappers_.end()) {
    LOG_ERROR("unregistered client was disconnected");
  }
  it->second.status = USER_STATUS_DOWN;
}

}  // namespace raft
}  // namespace abeille
