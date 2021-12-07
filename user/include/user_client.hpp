#ifndef ABEILLE_USER_CLIENT_H_
#define ABEILLE_USER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "abeille.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;

namespace abeille {
namespace user {

class Client {
 public:
  Client() = default;
  explicit Client(const std::vector<std::string> &addresses) noexcept
      : addresses_(addresses) {}
  ~Client() = default;

  Client &operator=(Client &&other) noexcept {
    stub_ptr_ = std::move(other.stub_ptr_);
    task_ids_ = other.task_ids_;
    results_ = other.results_;
    address_index_ = other.address_index_;
    addresses_ = other.addresses_;
    return *this;
  }

  UploadDataResponse UploadData(TaskData *task_data);

  TaskResult GetResult(uint64_t task_id);

 private:
  void createStub(const std::string &address);

  void connect();

  bool pingRemote();

  UploadDataResponse uploadData(TaskData *task_data);

  std::unique_ptr<UserService::Stub> stub_ptr_ = nullptr;

  std::vector<uint64_t> task_ids_;
  std::unordered_map<uint64_t, int> results_;

  size_t address_index_ = 0;
  std::vector<std::string> addresses_;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_CLIENT_H_
