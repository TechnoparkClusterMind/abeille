
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

Status UserServiceImpl::Connect(ServerContext *context, ConnectStream *stream) {
  std::string address = ExtractAddress(context->peer());
  uint64_t user_id = address2uint(address);
  LOG_INFO("user connection request from [%s]", address.c_str());

  UserConnectRequest request;
  users_.insert(user_id);

  while (stream->Read(&request)) {
    UserConnectResponse response;
    response.set_leader_id(leader_id_);

    // check if we are the leader
    if (!isLeader()) {
      LOG_INFO("redirecting [%s] to the leader...", address.c_str());
      response.set_command(USER_COMMAND_REDIRECT);
      stream->Write(response);
      break;
    }

    if (request.status() == USER_STATUS_UPLOAD_DATA) {
      if (request.has_task_data()) {
        auto upload_data_response = new UploadDataResponse;
        task_mgr_->UploadData(new TaskData(request.task_data()),
                              upload_data_response);
        response.set_allocated_upload_data_response(upload_data_response);
      } else {
        LOG_ERROR("user status upload data, but null task data");
      }
    }

    if (!stream->Write(response)) {
      break;
    }

    LOG_DEBUG("[%s] is [%s]", address.c_str(),
              UserStatus_Name(request.status()).c_str());

    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  LOG_WARN("connection with user [%s] was lost", address.c_str());

  return Status::OK;
}

// Status UserServiceImpl::GetResult(ServerContext *context,
//                                   const GetResultRequest *request,
//                                   GetResultResponse *response) {
//   LOG_DEBUG("request for [%u]", request->task_id());
//   TaskResult *task_result = new TaskResult();
//   task_result->set_result(7);
//   response->set_allocated_task_result(task_result);
//   return Status::OK;
// }

}  // namespace raft_node
}  // namespace abeille
