#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"
#include "macroses.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

namespace rpc {
class UserServiceImpl final : public UserService::Service {
  Status Ping(UNUSED ServerContext *context, const Empty *request,
              Empty *response) override;
  Status
  StreamData(UNUSED ServerContext *context,
             ServerReaderWriter<UserResponse, UserRequest> *stream) override;
};

} // namespace rpc
} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RPC_USER_SERVICE_H
