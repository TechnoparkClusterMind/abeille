#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <unordered_map>

#include "abeille.grpc.pb.h"
#include "errors.hpp"
#include "raft_consensus.hpp"
#include "service.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

using WorkerServiceSpec =
    abeille::rpc::Service<WorkerConnectRequest, WorkerConnectResponse,
                          WorkerService::Service>;

class WorkerServiceImpl final : public WorkerServiceSpec {
 public:
  using ConnReq = WorkerConnectRequest;
  using ConnResp = WorkerConnectResponse;
  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;

  struct ClientWrapper {
    Task task;
    WorkerStatus status = WORKER_STATUS_IDLE;
    WorkerCommand command = WORKER_COMMAND_NONE;
  };

  explicit WorkerServiceImpl(RaftConsensusPtr raft_consensus) noexcept
      : raft_consensus_(raft_consensus){};

  void CommandHandler(uint64_t client_id, ConnResp *resp) override;
  void StatusHandler(uint64_t client_id, const ConnReq *req) override;

  error AssignTask(uint64_t task_id, uint64_t &worker_id);
  error SendTask(const Task &task);
  error GetWorkerResult(uint64_t worker_id, TaskResult *task_result);

 private:
  void redirectToLeader(ConnResp *resp);
  void handleCommandAssign(ClientWrapper &cw, ConnResp *resp);
  void handleCommandProcess(ClientWrapper &cw, ConnResp *resp);
  void handleStatusCompleted(ClientWrapper &cw, const ConnReq *req);

 private:
  std::unordered_map<uint64_t, ClientWrapper> clients_;
  RaftConsensusPtr raft_consensus_ = nullptr;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
