#ifndef ABEILLE_RPC_RAFT_SERVER_H
#define ABEILLE_RPC_RAFT_SERVER_H

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

#include "user_service.hpp"

namespace rpc {
class RaftServer {
 public:
  RaftServer() = default;
  explicit RaftServer(const std::string& host) noexcept : host_(host) {}
  ~RaftServer() = default;

  void Run();

 private:
  std::string host_;
  std::unique_ptr<grpc::Server> server_ = nullptr;
};
}  // namespace rpc

#endif  // ABEILLE_RPC_RAFT_SERVER_H
