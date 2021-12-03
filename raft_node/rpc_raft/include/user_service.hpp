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
 public:
  UserServiceImpl() = default;
  explicit UserServiceImpl(std::shared_ptr<TaskManager> task_mgr) : task_mgr_(task_mgr){};

 private:
  bool IsLeader() const noexcept;

  Status Ping(ServerContext *context, const Empty *request, Empty *response) override;

  Status UploadData(ServerContext *context, const UploadDataRequest *request, UploadDataResponse *response) override;

  Status GetResult(ServerContext *context, const GetResultRequest *request, GetResultResponse *response) override;

  std::shared_ptr<TaskManager> task_mgr_;

  uint64_t id_ = 0;
  uint64_t leader_id_ = 0;
  uint64_t last_task_id_ = 0;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
