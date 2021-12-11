#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <queue>
#include <unordered_map>
#include <vector>

#include "raft/consensus/include/raft_consensus.hpp"
#include "rpc/include/service.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/errors.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft {

using WorkerServiceSpec =
    abeille::rpc::Service<WorkerConnectRequest, WorkerConnectResponse,
                          WorkerService::Service>;

class WorkerServiceImpl final : public WorkerServiceSpec {
 public:
  struct ClientWrapper {
    Task task;
    uint64_t payload = 0;
    std::queue<WorkerCommand> commands;
    WorkerStatus status = WORKER_STATUS_IDLE;
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
  error ProcessTask(const Task &task);
  error GetResult(uint64_t worker_id, TaskResult *task_result);

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

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
