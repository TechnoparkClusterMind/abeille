#ifndef ABEILLE_WORKER_CLIENT_H_
#define ABEILLE_WORKER_CLIENT_H_

#include <grpcpp/grpcpp.h>

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
  using ConnectStream = grpc::ClientReaderWriter<Empty, Empty>;

  Client() = default;
  explicit Client(const std::string address) noexcept : address_(address) {}
  ~Client() = default;

  error Run();

  void Shutdown();

 private:
  void createStub();

  void connect();

  std::string address_;

  std::unique_ptr<WorkerService::Stub> stub_ptr_ = nullptr;

  std::unique_ptr<ConnectStream> connect_stream_ptr_ = nullptr;

  std::unique_ptr<std::thread> connect_thread_ = nullptr;

  bool shutdown_ = false;
};

}  // namespace worker
}  // namespace abeille

#endif  // ABEILLE_WORKER_CLIENT_H_
