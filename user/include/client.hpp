#ifndef ABEILLE_USER_CLIENT_H
#define ABEILLE_USER_CLIENT_H

#include <grpcpp/grpcpp.h>

#include <memory>
#include <thread>
#include <vector>

#include "abeille.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;

namespace User {
class Client {
 public:
  Client() = default;
  explicit Client(const std::string& host) noexcept : host_(host) {}
  ~Client() = default;

  void StreamData(const std::vector<std::vector<int>>& data);

 private:
  // connect initializes stub if it's not set yet (is nullptr)
  void connect();

  bool pingRemote();

  void createStub();

  void streamData(const std::vector<std::vector<int>>& arr);

  std::string host_;
  std::unique_ptr<ClientContext> ctx_ptr_ = nullptr;
  std::unique_ptr<UserService::Stub> stub_ptr_ = nullptr;
};
}  // namespace User

#endif  // ABEILLE_USER_CLIENT_H
