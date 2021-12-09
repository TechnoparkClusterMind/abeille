#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <unordered_map>

#include "abeille.grpc.pb.h"
#include "raft_consensus.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

struct WorkerWrapper {
  Task *task;
  WorkerStatus status = WORKER_STATUS_IDLE;
  WorkerCommand command = WORKER_COMMAND_NONE;
};

class WorkerServiceImpl final : public WorkerService::Service {
 public:
  using ConnectStream =
      grpc::ServerReaderWriter<WorkerConnectResponse, WorkerConnectRequest>;

  using RaftConsensusPtr = std::shared_ptr<RaftConsensus>;

  explicit WorkerServiceImpl(RaftConsensusPtr raft_consensus) noexcept
      : raft_consensus_(raft_consensus){};

  Status AssignTask(const AssignTaskRequest *request,
                    AssignTaskResponse *response);

  Status SendTask(Task *task, SendTaskResponse *response);

  Status GetWorkerResult(const GetWorkerResultRequest *request,
                         GetWorkerResultResponse *response);

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override;

  std::unordered_map<uint64_t, WorkerWrapper> workers_;

  RaftConsensusPtr raft_consensus_ = nullptr;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
