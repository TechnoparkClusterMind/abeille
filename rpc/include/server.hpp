#ifndef ABEILLE_RPC_SERVER_H_
#define ABEILLE_RPC_SERVER_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "errors.hpp"

namespace abeille {
namespace rpc {

class Server {
 public:
  Server() = default;
  Server(const std::vector<std::string>& hosts, const std::vector<grpc::Service*>& services) noexcept;

  Server& operator=(Server&& other) noexcept;

  error Run();
  void Wait();
  void Shutdown();

 private:
  void init();
  void launch_and_wait();
  inline bool is_ready() const noexcept { return ready; }

  std::vector<std::string> hosts_;
  std::vector<grpc::Service*> services_;

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
