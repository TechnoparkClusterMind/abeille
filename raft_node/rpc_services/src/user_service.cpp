#include "user_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

Status rpc::UserServiceImpl::Ping(ServerContext *context, const Empty *request,
                                  Empty *response) {
  return Status::OK;
}

Status rpc::UserServiceImpl::StreamData(
    ServerContext *context,
    ServerReaderWriter<UserResponse, UserRequest> *stream) {
  UserRequest input_data;
  while (stream->Read(&input_data)) {
    UserResponse output_data;
    output_data.set_result(
        input_data.task_id()); // FIXME: actually perform work, send data to
                               // task manager
    stream->Write(output_data);
  }
  return Status::OK;
}

} // namespace raft_node
} // namespace abeille
