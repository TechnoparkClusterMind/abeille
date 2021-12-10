#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <unordered_map>
#include <vector>

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
  struct ClientWrapper {
    Task task;
    uint64_t payload = 0;
    WorkerStatus status = WORKER_STATUS_IDLE;
    WorkerCommand command = WORKER_COMMAND_NONE;
  };

  using ConnReq = WorkerConnectRequest;
  using ConnResp = WorkerConnectResponse;
  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;
  using ClientsMap = std::unordered_map<uint64_t, ClientWrapper>;

  explicit WorkerServiceImpl(RaftConsensusPtr raft_consensus) noexcept
      : raft_consensus_(raft_consensus){};

  void ConnectHandler(uint64_t client_id) override;
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
  void handleStatusCompleted(ClientWrapper &cw, const ConnReq *req);

 private:
  ClientsMap client_wrappers_;

  size_t curr_client_id_ = 0;
  std::vector<uint64_t> client_ids_;

  RaftConsensusPtr raft_consensus_ = nullptr;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
