#include "raft_server.h"

#include <string>
#include <thread>

namespace Abeille {
namespace Raft_node {

Server::Server(const std::string& _address, grpc::Service* service)
    : address(_address), ready(false), server(nullptr) {
    // Listen on the given address without any auth mechanism
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(service);
}

Status Server::run() {
    std::thread(&Server::launch_and_wait).detach();

    std::unique_lock<std::mutex> lk(mut);
    cv.wait(lk, is_ready());

    return (server == nullptr) ? Status::Code::FAILURE : Status::Code::OK;
}

void Server::launch_and_wait() {
    server = builder.BuildAndStart();

    std::lock_guard<std::mutex> lk(mut);
    ready = true;
    cv.notify_one();

    if (server == nullptr)
        return;

    server->Wait();
}

}  // namespace Raft_node
}  // namespace Abeille
