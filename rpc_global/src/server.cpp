#include "server.hpp"

namespace abeille {
namespace rpc {

Server::Server(const std::vector<std::string>& hosts, const std::vector<grpc::Service*>& services) noexcept
    : hosts_(hosts), services_(services) {}

Server& Server::operator=(Server&& other) noexcept {
  if (this != &other) {
    hosts_ = std::move(other.hosts_);
    services_ = std::move(other.services_);

    thread_ = std::move(other.thread_);
    server_ = std::move(other.server_);
  }
  return *this;
}

error Server::Run() {
  init();

  thread_ = std::make_unique<std::thread>(std::thread(&Server::launch_and_wait, this));

  std::unique_lock<std::mutex> lk(mut);
  cv.wait(lk, [&] { return ready; });

  if (server_ == nullptr) {
    return error(error::Code::FAILURE);
  }

  return error();
}

void Server::Wait() { server_->Wait(); }

void Server::Shutdown() {
  std::cout << "Shutting down..." << std::endl;
  server_->Shutdown();
  thread_->join();
}

void Server::init() {
  for (const std::string& host : hosts_) {
    std::cout << "Starting listening " << host << std::endl;
    builder_.AddListeningPort(host, grpc::InsecureServerCredentials());
  }

  std::cout << "Registering services..." << std::endl;
  for (const auto service : services_) {
    builder_.RegisterService(service);
  }

  std::cout << "Finished server initialization" << std::endl;
}

void Server::launch_and_wait() {
  {
    std::lock_guard<std::mutex> lk(mut);
    server_ = builder_.BuildAndStart();
    ready = true;
  }
  cv.notify_one();
  if (server_ != nullptr) {
    server_->Wait();
  }
}

}  // namespace rpc
}  // namespace abeille
