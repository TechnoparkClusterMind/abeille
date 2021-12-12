#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "raft/consensus/include/raft_consensus.hpp"
#include "raft/task_manager/include/task_manager.hpp"
#include "rpc/include/service.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/types.hpp"

using grpc::ServerContext;
using grpc::Status;

namespace abeille {
namespace raft {

class TaskManager;
class RaftConsensus;

using UserServiceSpec = abeille::rpc::Service<UserConnectRequest, UserConnectResponse, UserService::Service>;

class UserServiceImpl final : public UserServiceSpec {
 public:
  using ConnReq = UserConnectRequest;
  using ConnResp = UserConnectResponse;
  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;
  using TaskManagerPtr = std::shared_ptr<TaskManager>;

  struct ClientWrapper {
    uint64_t client_id = 0;
    std::queue<TaskState> task_state_queue;
    std::queue<UserCommand> commands;
    UserStatus status = USER_STATUS_IDLE;
  };

  UserServiceImpl(RaftConsensusPtr raft_consensus, TaskManagerPtr task_mgr) noexcept
      : raft_consensus_(raft_consensus), task_mgr_(task_mgr){};

  void ConnectHandler(uint64_t client_id) override;
  void CommandHandler(uint64_t client_id, ConnResp &resp) override;
  void StatusHandler(uint64_t client_id, const ConnReq &req) override;
  void DisconnectHandler(uint64_t client_id) override;

  error SendTaskResult(const TaskWrapper &task_wrapper);

 private:
  error handleCommandAssign(ClientWrapper &cw, ConnResp &resp);
  error handleCommandResult(ClientWrapper &cw, ConnResp &resp);

  error handleStatusUploadData(ClientWrapper &cw, const ConnReq &req);
  error handleStatusCompleted(ClientWrapper &cw, const ConnReq &req);

 private:
  RaftConsensusPtr raft_consensus_ = nullptr;
  TaskManagerPtr task_mgr_ = nullptr;

  std::unordered_map<uint64_t, ClientWrapper> client_wrappers_;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
