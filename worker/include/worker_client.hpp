#ifndef ABEILLE_WORKER_CLIENT_H_
#define ABEILLE_WORKER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "abeille.grpc.pb.h"
#include "errors.hpp"

using grpc::Channel;
using grpc::ClientContext;

namespace abeille {
namespace worker {

class Client {
 public:
  using ConnectStream = grpc::ClientReaderWriter<WorkerStatus, ConnectResponse>;

  Client() = default;
  explicit Client(const std::string address) noexcept : address_(address) {}
  ~Client() = default;

  error Run();

  void Shutdown();

  error ProcessTaskData(const TaskData &task_data);

 private:
  void createStub();

  void connect();

  void keepAlive();

  bool handshake();

 private:
  bool shutdown_ = false;
  bool connected_ = false;
  NodeStatus status_ = NodeStatus::IDLE;

  std::string address_;
  uint64_t leader_id_ = 0;

  std::mutex mutex_;
  std::condition_variable cv_;

  uint64_t task_id_ = 0;
  TaskResult *task_result_;

  std::thread connect_thread_;
  std::unique_ptr<ClientContext> connect_ctx_ = nullptr;
  std::unique_ptr<ConnectStream> connect_stream_ = nullptr;
  std::unique_ptr<WorkerService::Stub> stub_ptr_ = nullptr;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_WORKER_CLIENT_H_
