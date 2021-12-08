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

using WorkerClient = abeille::rpc::Client<WorkerConnectRequest,
                                          WorkerConnectResponse, WorkerService>;

class Client : public WorkerClient {
 public:
  Client(const std::string &address) noexcept : WorkerClient(address) {
    std::vector<WorkerClient::CommandHandler> command_handlers(
        WorkerCommand_ARRAYSIZE);

    command_handlers[WORKER_COMMAND_NONE] =
        std::bind(&Client::handleCommandNone, this, std::placeholders::_1);

    command_handlers[WORKER_COMMAND_ASSIGN] =
        std::bind(&Client::handleCommandAssign, this, std::placeholders::_1);

    command_handlers[WORKER_COMMAND_PROCESS] =
        std::bind(&Client::handleCommandProcess, this, std::placeholders::_1);

    command_handlers[WORKER_COMMAND_REDIRECT] =
        std::bind(&Client::handleCommandRedirect, this, std::placeholders::_1);

    SetCommandHandlers(command_handlers);
  }

 private:
  void handleCommandNone(const WorkerConnectResponse *response);
  void handleCommandAssign(const WorkerConnectResponse *response);
  void handleCommandProcess(const WorkerConnectResponse *response);
  void handleCommandRedirect(const WorkerConnectResponse *response);

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
