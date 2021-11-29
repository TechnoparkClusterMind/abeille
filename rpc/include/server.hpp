#ifndef ABEILLE_RPC_RAFT_SERVER_H
#define ABEILLE_RPC_RAFT_SERVER_H

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

using grpc::Status;

namespace abeille {
namespace rpc {

// Depending on service:
// * Raft server (Raft_service)
// * User server (User_service)
// * Worker server (Worker_service)
class Server {
public:
  // Initialize server
  // Service is allocated by Core
  // (in order to get rid of cyclic dependency)
  explicit Server(const std::string &address,
                  std::unique_ptr<grpc::Service> service);

  // async run in detached thread launch_and_wait
  Status Run();

  // shutdown server
  void Shutdown();

private:
  // Maybe we'll add some new logic in the future
  std::unique_ptr<grpc::Server> server_;
  grpc::ServerBuilder builder_;
  std::string address_;

  bool ready_;
  std::condition_variable cv_;
  std::mutex mut_;

  // Launch and wait (supposed to be invoked async)
  void Launch();
};

} // namespace rpc
} // namespace abeille

#endif // ABEILLE_RPC_RAFT_SERVER_H
