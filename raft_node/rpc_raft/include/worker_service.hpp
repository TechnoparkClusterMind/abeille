#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <unordered_map>

#include "abeille.grpc.pb.h"

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

  Status AssignTask(const AssignTaskRequest *request,
                    AssignTaskResponse *response);

  Status SendTask(Task *task, SendTaskResponse *response);

  Status GetWorkerResult(const GetWorkerResultRequest *request,
                         GetWorkerResultResponse *response);

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override;

  bool isLeader() const noexcept { return id_ == leader_id_; }

  std::unordered_map<uint64_t, WorkerWrapper> workers_;

  uint64_t id_ = 0;
  uint64_t leader_id_ = 0;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
