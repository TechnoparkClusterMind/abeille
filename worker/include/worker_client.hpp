#ifndef ABEILLE_WORKER_CLIENT_H_
#define ABEILLE_WORKER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "rpc/include/client.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "user/model/proto/task.pb.h"
#include "utils/include/errors.hpp"
#include "utils/include/logger.hpp"
#include "utils/include/types.hpp"

using grpc::Channel;
using grpc::ClientContext;
using namespace std::placeholders;

namespace abeille {
namespace worker {

using ConnReq = WorkerConnectRequest;
using ConnResp = WorkerConnectResponse;
using WorkerClient = abeille::rpc::Client<ConnReq, ConnResp, WorkerService>;

class Client : public WorkerClient {
 public:
  Client(const std::vector<std::string> &cluster_addresses) noexcept : WorkerClient(cluster_addresses) {}

  void CommandHandler(const ConnResp &resp) override;
  void StatusHandler(ConnReq &req) override;

 private:
  void handleCommandAssign(const ConnResp &resp);
  void handleCommandProcess(const ConnResp &resp);
  void handleCommandRedirect(const ConnResp &resp);

  void handleStatusCompleted(ConnReq &req);
  void processTaskData(const Bytes &task_data);

 private:
  TaskID task_id_;
  uint64_t leader_id_ = 0;
  Bytes task_result_;
  WorkerStatus status_ = WORKER_STATUS_IDLE;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_WORKER_CLIENT_H_
