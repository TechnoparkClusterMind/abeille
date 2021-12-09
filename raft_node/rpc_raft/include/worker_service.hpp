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

struct WorkerWrapper {
  Task task;
  WorkerStatus status = WORKER_STATUS_IDLE;
  WorkerCommand command = WORKER_COMMAND_NONE;
};

using ConnReq = WorkerConnectRequest;
using ConnResp = WorkerConnectResponse;

using WorkerServiceSpec =
    abeille::rpc::Service<ConnReq, ConnResp, WorkerService::Service>;

class WorkerServiceImpl final : public WorkerServiceSpec {
 public:
  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;

  explicit WorkerServiceImpl(RaftConsensusPtr raft_consensus) noexcept
      : raft_consensus_(raft_consensus){};

  void CommandHandler(uint64_t client_id, ConnResp *resp) override;

  void StatusHandler(uint64_t client_id, const ConnReq *req) override;

  error AssignTask(uint64_t task_id, uint64_t &worker_id);

  error SendTask(const Task &task);

  error GetWorkerResult(uint64_t worker_id, TaskResult *task_result);

 private:
  void redirectToLeader(ConnResp *resp);

  void handleCommandAssign(WorkerWrapper &worker, ConnResp *resp);

  void handleCommandProcess(WorkerWrapper &worker, ConnResp *resp);

  void handleStatusCompleted(WorkerWrapper &worker, const ConnReq *req);

 private:
  std::unordered_map<uint64_t, WorkerWrapper> workers_;
  RaftConsensusPtr raft_consensus_ = nullptr;
};

// class WorkerServiceImpl final : public WorkerService::Service {
//  public:
//   using ConnectStream =
//       grpc::ServerReaderWriter<WorkerConnectResponse, WorkerConnectRequest>;

//   using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;

//   explicit WorkerServiceImpl(RaftConsensusPtr raft_consensus) noexcept
//       : raft_consensus_(raft_consensus){};

//   Status AssignTask(const AssignTaskRequest *request,
//                     AssignTaskResponse *response);

//   Status SendTask(Task *task, SendTaskResponse *response);

//   Status GetWorkerResult(const GetWorkerResultRequest *request,
//                          GetWorkerResultResponse *response);

//  private:
//   Status Connect(ServerContext *context, ConnectStream *stream) override;

//   std::unordered_map<uint64_t, WorkerWrapper> workers_;

//   RaftConsensusPtr raft_consensus_ = nullptr;
// };

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
