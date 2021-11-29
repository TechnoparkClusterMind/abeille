#include "server.hpp"

using grpc::Status;
using grpc::StatusCode;

namespace abeille {

rpc::Server::Server(const std::string &address,
                    std::unique_ptr<grpc::Service> service)
    : server_(nullptr), address_(address), ready_(false) {
  // Listen on the given address without any auth mechanism
  builder_.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder_.RegisterService(service.get());
}

Status rpc::Server::Run() {
  std::thread(&Server::Launch, this).detach();

  std::unique_lock<std::mutex> lk(mut_);
  while (!ready_)
    cv_.wait(lk);

  return (server_ == nullptr) ? Status(StatusCode::UNKNOWN, 
      "Port or service are incorrect") : Status::OK;
}

void rpc::Server::Launch() {
  server_ = builder_.BuildAndStart();

  std::lock_guard<std::mutex> lk(mut_);
  ready_ = true;
  cv_.notify_one();

  if (server_ == nullptr)
    return;

  server_->Wait();
}

void rpc::Server::Shutdown() { server_->Shutdown(); }

} // namespace abeille
