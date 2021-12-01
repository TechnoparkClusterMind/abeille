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
namespace rpc {

Status UserServiceImpl::Ping(ServerContext *context, const Empty *request, Empty *response) { return Status::OK; }

Status UserServiceImpl::Upload(ServerContext *context, const UploadRequest *request, UploadResponse *response) {
  // FIXME: actually perform work, send data to task manager
  response->set_task_id(42);
  return Status::OK;
}

}  // namespace rpc
}  // namespace abeille
