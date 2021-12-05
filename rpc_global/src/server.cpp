#include "server.hpp"

#include "logger.hpp"

namespace abeille {
namespace rpc {

Server::Server(const std::vector<std::string> &hosts, const std::vector<grpc::Service *> &services) noexcept
    : hosts_(hosts), services_(services) {}

Server::Server(Server &&other) noexcept
    : hosts_(std::move(other.hosts_)),
      services_(std::move(other.services_)),
      thread_(std::move(other.thread_)),
      server_(std::move(other.server_)) {}

Server &Server::operator=(Server &&other) noexcept {
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

  LOG_INFO("launching the server...");
  thread_ = std::make_unique<std::thread>(std::thread(&Server::launch_and_wait, this));

  std::unique_lock<std::mutex> lk(mut);
  cv.wait(lk, [&] { return ready; });

  if (server_ == nullptr) {
    return error("failed to build and start the server");
  }

  LOG_INFO("server is running");
  return error();
}

void Server::Shutdown() {
  LOG_INFO("shutting down...");
  server_->Shutdown(std::chrono::system_clock::now() + SHUTDOWN_TIMEOUT);
  thread_->join();
}

void Server::init() {
  for (const std::string &host : hosts_) {
    LOG_INFO("starting listening %s", host.c_str());
    builder_.AddListeningPort(host, grpc::InsecureServerCredentials());
  }

  LOG_INFO("registering services...");
  for (const auto service : services_) {
    builder_.RegisterService(service);
  }
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
