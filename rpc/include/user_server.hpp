#ifndef ABEILLE_RPC_USER_SERVER_H
#define ABEILLE_RPC_USER_SERVER_H

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

namespace rpc {
class UserServer {
 public:
  UserServer() = default;
  explicit UserServer(const std::string& host) noexcept : host_(host) {}
  ~UserServer() = default;

  void Run();

 private:
  std::string host_;
  std::unique_ptr<grpc::Server> server_ = nullptr;
};
}  // namespace rpc

#endif  // ABEILLE_RPC_USER_SERVER_H
