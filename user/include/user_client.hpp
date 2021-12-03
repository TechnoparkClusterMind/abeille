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
  explicit Client(const std::string &host) noexcept : host_(host) {}
  ~Client() = default;

  UploadDataResponse UploadData(TaskData *task_data);

  TaskResult GetResult(uint64_t task_id);

 private:
  void createStub();

  void connect();

  bool pingRemote();

  UploadDataResponse uploadData(TaskData *task_data);

  std::string host_;
  std::unique_ptr<UserService::Stub> stub_ptr_ = nullptr;

  std::vector<uint64_t> task_ids_;
  std::unordered_map<uint64_t, int> results_;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_CLIENT_H_
