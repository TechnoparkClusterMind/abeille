#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {

namespace rpc {
class UserServiceImpl final : public UserService::Service {
  Status Ping(ServerContext *context, const Empty *request, Empty *response) override;
  Status Upload(ServerContext *context, const UploadRequest *request, UploadResponse *response) override;
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
