#include "worker_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace worker {

error Client::Run() {
  createStub();

  LOG_INFO("connecting to the server...");
  connect_thread_ = std::thread(&Client::connect, this);

  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [&] { return connected_ || shutdown_; });

  return error();
}

void Client::Shutdown() {
  LOG_INFO("shutting down...");
  shutdown_ = true;
  if (connect_thread_.joinable()) {
    connect_thread_.join();
  }
}

void Client::createStub() {
  auto channel = grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());
  stub_ptr_ = WorkerService::NewStub(channel);
}

void Client::connect() {
  {
    // try to connect to the server; last stream and context are preserved
    std::lock_guard<std::mutex> lk(mutex_);
    while (!handshake() && !shutdown_) {
      LOG_ERROR("failed to connect to the server, retrying...");
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // if exited because of shutdown, notify and return to successfully shutdown
    if (shutdown_) {
      cv_.notify_one();
      return;
    }

    LOG_INFO("successfully connected to the server");
    connected_ = true;
  }
  cv_.notify_one();

  keepAlive();
}

void Client::keepAlive() {
  // keep the connection alive: respond to beats from the server
  Empty response;
  WorkerStatus request;
  while (connect_stream_->Read(&response) && !shutdown_) {
    request.set_status(status_);
    connect_stream_->Write(request);
  }

  connected_ = false;
  connect_stream_->WritesDone();
  LOG_INFO("disconnected from the server...");

  // if not shutdown, we need to try to reconnect
  if (!shutdown_) {
    connect();
  }
}

bool Client::handshake() {
  WorkerStatus request;
  request.set_status(status_);

  connect_ctx_ = std::make_unique<ClientContext>();
  connect_stream_ = stub_ptr_->Connect(connect_ctx_.get());

  return connect_stream_->Write(request);
}

}  // namespace worker
}  // namespace abeille
