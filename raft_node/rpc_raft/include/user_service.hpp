#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "abeille.grpc.pb.h"
#include "task_manager.hpp"

using grpc::ServerContext;
using grpc::Status;

namespace abeille {
namespace raft_node {

class TaskManager;

class UserServiceImpl final : public UserService::Service {
 public:
  using ConnectStream =
      grpc::ServerReaderWriter<UserConnectResponse, UserConnectRequest>;

  UserServiceImpl() = default;
  explicit UserServiceImpl(std::shared_ptr<TaskManager> task_mgr)
      : task_mgr_(task_mgr){};

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override;

  bool isLeader() const noexcept { return id_ == leader_id_; }

  std::shared_ptr<TaskManager> task_mgr_;

  uint64_t id_ = 0;
  uint64_t leader_id_ = 0;

  std::unordered_set<uint64_t> users_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
