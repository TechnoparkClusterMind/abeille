#ifndef ABEILLE_USER_CLIENT_H_
#define ABEILLE_USER_CLIENT_H_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "abeille.grpc.pb.h"
#include "client.hpp"
#include "errors.hpp"

using grpc::Channel;
using grpc::ClientContext;

namespace abeille {
namespace user {

using ConnReq = UserConnectRequest;
using ConnResp = UserConnectResponse;
using UserClient = abeille::rpc::Client<ConnReq, ConnResp, UserService>;

class Client : public UserClient {
 public:
  Client(const std::string &address) noexcept : UserClient(address) {}

  void CommandHandler(const ConnResp &resp) override;

  void StatusHandler(ConnReq &req) override;

  error UploadData(TaskData *task_data);

 private:
  void handleCommandRedirect(const ConnResp &resp);
  void handleCommandAssign(const ConnResp &resp);
  void handleCommandResult(const ConnResp &resp);

  void handleStatusUploadData(ConnReq &req);

 private:
  std::mutex mutex_;
  std::condition_variable cv_;

  uint64_t leader_id_ = 0;
  UserStatus status_ = USER_STATUS_IDLE;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_CLIENT_H_
