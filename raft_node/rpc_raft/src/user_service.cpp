
#include "user_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "abeille.grpc.pb.h"
#include "convert.hpp"
#include "logger.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

void UserServiceImpl::CommandHandler(uint64_t client_id, ConnResp *resp) {
  resp->set_leader_id(raft_consensus_->LeaderID());

  // check if we are the leader
  if (!raft_consensus_->IsLeader()) {
    LOG_INFO("redirecting [%s] to the leader...",
             uint2address(client_id).c_str());
    resp->set_command(USER_COMMAND_REDIRECT);
    return;
  }

  auto &cw = client_wrappers_[client_id];
  resp->set_command(cw.command);

  switch (cw.command) {
    case USER_COMMAND_ASSIGN:
      handleCommandAssign(cw, resp);
    case USER_COMMAND_RESULT:
    default:
      break;
  }

  // reset command so to execute a command only once
  cw.command = USER_COMMAND_NONE;
}

void UserServiceImpl::handleCommandAssign(ClientWrapper &cw, ConnResp *resp) {
  resp->set_task_id(cw.task_state.task_id());
}

void UserServiceImpl::StatusHandler(uint64_t client_id, const ConnReq *req) {
  auto &cw = client_wrappers_[client_id];

  // update the status of the client
  cw.status = req->status();

  switch (cw.status) {
    case USER_STATUS_UPLOAD_DATA:
      handleStatusUploadData(cw, req);
      break;
    default:
      break;
  }
}

void UserServiceImpl::handleStatusUploadData(ClientWrapper &cw,
                                             const ConnReq *req) {
  if (req->has_task_data()) {
    uint64_t task_id = 0;
    task_mgr_->UploadData(req->task_data(), task_id);
    cw.task_state.set_task_id(task_id);
    cw.command = USER_COMMAND_ASSIGN;
  } else {
    LOG_ERROR("user status upload data, but null task data");
  }
}

void UserServiceImpl::DisconnectHandler(uint64_t client_id) {
  auto it = client_wrappers_.find(client_id);
  if (it == client_wrappers_.end()) {
    LOG_ERROR("unregistered client was disconnected");
  }
  it->second.status = USER_STATUS_DOWN;
}

}  // namespace raft_node
}  // namespace abeille
