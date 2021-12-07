#ifndef ABEILLE_RPC_SERVER_H_
#define ABEILLE_RPC_SERVER_H_

#include <grpcpp/grpcpp.h>

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "errors.hpp"

namespace abeille {
namespace rpc {

static constexpr const auto SHUTDOWN_TIMEOUT = std::chrono::seconds(5);

struct ServiceInfo {
  std::string address;
  grpc::Service *service;
};

class Server {
 public:
  Server() = default;
  Server(std::vector<ServiceInfo> services) noexcept : services_(services) {}

  Server(Server &&other) noexcept;
  Server &operator=(Server &&other) noexcept;

  error Run();

  void Shutdown();

 private:
  void init();
  void launch_and_wait();
  inline bool is_ready() const noexcept { return ready; }

 private:
  std::vector<ServiceInfo> services_;

  grpc::ServerBuilder builder_;
  std::unique_ptr<std::thread> thread_ = nullptr;
  std::unique_ptr<grpc::Server> server_ = nullptr;

  std::mutex mut;
  bool ready = false;
  std::condition_variable cv;
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_SERVER_H_
