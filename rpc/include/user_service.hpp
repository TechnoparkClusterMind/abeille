#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace rpc {
class UserServiceImpl final : public UserService::Service {
  Status Ping(ServerContext* context, const Empty* request, Empty* response) override;
  Status StreamData(ServerContext* context, ServerReaderWriter<UserResponse, UserRequest>* stream) override;
};
}  // namespace rpc

#endif  // ABEILLE_RPC_USER_SERVICE_H
