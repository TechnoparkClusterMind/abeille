#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"
#include "task_manager.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

class UserServiceImpl final : public UserService::Service {
  std::shared_ptr<TaskManager> task_mgr_;

public:
  UserServiceImpl() = default;
  explicit UserServiceImpl(std::shared_ptr<TaskManager> task_mgr)
      : task_mgr_(task_mgr){};
  Status Ping(ServerContext *context, const Empty *request,
              Empty *response) override;
  Status Upload(ServerContext *context, const UploadRequest *request,
                UploadResponse *response) override;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RPC_USER_SERVICE_H
