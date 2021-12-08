#ifndef ABEILLE_WORKER_CLIENT_H_
#define ABEILLE_WORKER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "abeille.grpc.pb.h"
#include "client.hpp"
#include "errors.hpp"
#include "logger.hpp"

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
  Client(const std::string &address) noexcept : WorkerClient(address) {}

  void CommandsHandler(const ConnResp *resp) override;

 private:
  void handleCommandNone(const ConnResp *response);
  void handleCommandAssign(const ConnResp *response);
  void handleCommandProcess(const ConnResp *response);
  void handleCommandRedirect(const ConnResp *response);
  void handleCommandUnrecognized(const ConnResp *response);

  void processData(const TaskData &task_data);

 private:
  uint64_t task_id_ = 0;
  uint64_t leader_id_ = 0;
  TaskResult *task_result_ = nullptr;
  WorkerStatus status_ = WORKER_STATUS_IDLE;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_WORKER_CLIENT_H_
