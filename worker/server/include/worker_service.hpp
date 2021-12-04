#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include <unordered_map>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace worker {

class WorkerServiceImpl final : public WorkerService::Service {
 public:
  using ConnectStream = grpc::ServerReaderWriter<Empty, WorkerStatus>;

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override;

  std::unordered_map<uint64_t, NodeStatus> workers_;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
