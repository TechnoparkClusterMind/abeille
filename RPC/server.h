#ifndef ABEILLE_RAFT_RAFT_SERVER_H
#define ABEILLE_RAFT_RAFT_SERVER_H
#include <string>
#include <grpcpp/grpcpp.h>
#include <memory>
#include "status.h"
#include <condition_variable>
#include <mutex>

namespace Abeille {
namespace Raft_node {

// Depending on service:
// * Raft server (Raft_service)
// * User server (User_service)
// * Worker server (Worker_service)
class Server {
   public:
    // Initialize server
    explicit Server(const std::string& address, grpc::Service *service);

    // async run in detached thread launch_and_wait
    Status run();

    // shutdown server
    void shutdown();

   private:
    // Maybe we'll add some new logic in the future
    std::unique_ptr<grpc::Server> server;
    grpc::ServerBuilder builder;
    std::string address;

    bool ready;
    std::condition_variable cv;
    std::mutex mut;

    void launch_and_wait();
    bool is_ready() { return ready; }
};

}  // namespace Raft_node
}  // namespace Abeille

#endif  // ABEILLE_RAFT_RAFT_SERVER_H
