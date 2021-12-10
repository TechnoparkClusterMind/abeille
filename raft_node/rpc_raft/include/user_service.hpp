#ifndef ABEILLE_RPC_USER_SERVICE_H
#define ABEILLE_RPC_USER_SERVICE_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "abeille.grpc.pb.h"
#include "raft_consensus.hpp"
#include "service.hpp"
#include "task_manager.hpp"

using grpc::ServerContext;
using grpc::Status;

namespace abeille {
namespace raft_node {

class TaskManager;

using UserServiceSpec =
    abeille::rpc::Service<UserConnectRequest, UserConnectResponse,
                          UserService::Service>;

class UserServiceImpl final : public UserServiceSpec {
 public:
  using ConnReq = UserConnectRequest;
  using ConnResp = UserConnectResponse;
  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;
  using TaskManagerPtr = std::shared_ptr<TaskManager>;

  struct ClientWrapper {
    TaskState task_state;
    UserStatus status = USER_STATUS_IDLE;
    UserCommand command = USER_COMMAND_NONE;
  };

  UserServiceImpl(RaftConsensusPtr raft_consensus,
                  TaskManagerPtr task_mgr) noexcept
      : raft_consensus_(raft_consensus), task_mgr_(task_mgr){};

  void CommandHandler(uint64_t client_id, ConnResp *resp) override;
  void StatusHandler(uint64_t client_id, const ConnReq *req) override;
  void DisconnectHandler(uint64_t client_id) override;

  error AssignTask(uint64_t task_id, uint64_t &worker_id);
  error SendTask(const Task &task);
  error GetWorkerResult(uint64_t worker_id, TaskResult *task_result);

 private:
  void redirectToLeader(ConnResp *resp);
  void handleCommandAssign(ClientWrapper &cw, ConnResp *resp);
  void handleCommandProcess(ClientWrapper &cw, ConnResp *resp);

  void handleStatusUploadData(ClientWrapper &cw, const ConnReq *req);
  void handleStatusCompleted(ClientWrapper &cw, const ConnReq *req);

 private:
  RaftConsensusPtr raft_consensus_ = nullptr;
  TaskManagerPtr task_mgr_ = nullptr;

  std::unordered_map<uint64_t, ClientWrapper> client_wrappers_;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_USER_SERVICE_H
