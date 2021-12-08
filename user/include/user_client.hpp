#ifndef ABEILLE_USER_CLIENT_H_
#define ABEILLE_USER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "abeille.grpc.pb.h"
#include "errors.hpp"

using grpc::Channel;
using grpc::ClientContext;

namespace abeille {
namespace user {

class Client {
 public:
  using ConnectStream =
      grpc::ClientReaderWriter<UserConnectRequest, UserConnectResponse>;

  Client() = default;
  explicit Client(const std::vector<std::string> &addresses) noexcept;
  ~Client() = default;

  Client &operator=(Client &&other) noexcept {
    stub_ptr_ = std::move(other.stub_ptr_);
    task_ids_ = other.task_ids_;
    results_ = other.results_;
    address_index_ = other.address_index_;
    addresses_ = other.addresses_;
    return *this;
  }

  error Run();

  void Shutdown();

  void UploadData(TaskData *task_data);

  TaskResult GetResult(uint64_t task_id);

 private:
  void createStub();

  void connect();

  void keepAlive();

  bool handshake();

 private:
  bool shutdown_ = false;
  bool connected_ = false;
  UserStatus status_ = USER_STATUS_NONE;

  uint64_t leader_id_ = 0;
  std::string address_;

  size_t address_index_ = 0;
  std::vector<std::string> addresses_;

  std::mutex mutex_;
  std::condition_variable cv_;

  std::vector<uint64_t> task_ids_;
  std::unordered_map<uint64_t, int> results_;
  std::queue<TaskData *> task_datas_;

  std::thread connect_thread_;
  std::unique_ptr<ClientContext> connect_ctx_ = nullptr;
  std::unique_ptr<ConnectStream> connect_stream_ = nullptr;
  std::unique_ptr<UserService::Stub> stub_ptr_ = nullptr;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_CLIENT_H_
