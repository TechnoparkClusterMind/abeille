#ifndef ABEILLE_RPC_WORKER_SERVICE_H
#define ABEILLE_RPC_WORKER_SERVICE_H

#include <grpc/grpc.h>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {

class WorkerServiceImpl final : public WorkerService::Service {
  Status Connect(ServerContext *context, ServerReaderWriter<Empty, Empty> *stream) override;
};

}  // namespace abeille

#endif  // ABEILLE_RPC_WORKER_SERVICE_H
