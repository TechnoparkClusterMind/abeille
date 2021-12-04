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
  using ConnectStream = grpc::ClientReaderWriter<WorkerStatus, Empty>;

  Client() = default;
  explicit Client(const std::string address) noexcept : address_(address) {}
  ~Client() = default;

  error Run();

  void Shutdown();

 private:
  void createStub();

  void connect();

  void keepAlive();

  bool handshake();

  std::string address_;

  std::unique_ptr<WorkerService::Stub> stub_ptr_ = nullptr;

  std::thread connect_thread_;
  std::unique_ptr<ClientContext> connect_ctx_ = nullptr;
  std::unique_ptr<ConnectStream> connect_stream_ = nullptr;

  bool shutdown_ = false;
  bool connected_ = false;

  std::mutex mutex_;
  std::condition_variable cv_;

  NodeStatus status_ = NodeStatus::IDLE;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_WORKER_CLIENT_H_
