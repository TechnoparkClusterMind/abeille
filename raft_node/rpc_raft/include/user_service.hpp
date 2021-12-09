#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "abeille.grpc.pb.h"
#include "raft_consensus.hpp"
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

  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;
  using TaskManagerPtr = std::shared_ptr<TaskManager>;

  UserServiceImpl() = default;
  explicit UserServiceImpl(RaftConsensusPtr raft_consensus,
                           TaskManagerPtr task_mgr) noexcept
      : raft_consensus_(raft_consensus), task_mgr_(task_mgr){};

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override;

  RaftConsensusPtr raft_consensus_ = nullptr;
  TaskManagerPtr task_mgr_ = nullptr;

  std::unordered_set<uint64_t> users_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
