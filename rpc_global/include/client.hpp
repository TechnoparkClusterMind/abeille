#ifndef ABEILLE_RPC_CLIENT_HPP_
#define ABEILLE_RPC_CLIENT_HPP_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "errors.hpp"
#include "logger.hpp"

using grpc::ClientContext;

namespace abeille {
namespace rpc {

template <typename ConnReq, typename ConnResp, typename Svc>
class Client {
 public:
  using ConnStream = grpc::ClientReaderWriter<ConnReq, ConnResp>;
  using SvcStub = typename Svc::Stub;

  Client() = default;
  explicit Client(const std::string &address) noexcept : address_(address) {}
  ~Client() = default;

  virtual void CommandHandler(const ConnResp *resp) = 0;

  virtual void StatusHandler(ConnReq *req) = 0;

  error Run() {
    LOG_INFO("connecting to the server [%s]...", address_.c_str());
    std::thread(&Client::connect, this).detach();

    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait(lk, [&] { return connected_ || shutdown_; });

    return error();
  }

  void Shutdown() {
    LOG_INFO("shutting down...");
    shutdown_ = true;
  }

 protected:
  void createStub() {
    auto channel =
        grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());
    stub_ptr_ = Svc::NewStub(channel);
  }

  void connect() {
    createStub();
    {
      // try to connect to the server; last stream and context are preserved
      std::lock_guard<std::mutex> lk(mutex_);
      while (!shutdown_ && !handshake()) {
        LOG_ERROR("failed to connect to the server, retrying...");
        std::this_thread::sleep_for(std::chrono::seconds(3));
      }

      // if exited because of shutdown, notify and return to successfully
      // shutdown
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

  bool handshake() {
    if (shutdown_) {
      return false;
    }

    ConnReq req;
    connect_ctx_ = std::make_unique<ClientContext>();
    connect_stream_ = stub_ptr_->Connect(connect_ctx_.get());
    return connect_stream_->Write(req);
  }

  void keepAlive() {
    // keep the connection alive: respond to beats from the server
    auto resp = std::make_unique<ConnResp>();
    while (connect_stream_->Read(resp.get()) && !shutdown_) {
      CommandHandler(resp.get());

      auto req = std::make_unique<ConnReq>();
      StatusHandler(req.get());

      connect_stream_->Write(*req);
    }

    connected_ = false;
    connect_stream_->WritesDone();
    LOG_INFO("disconnected from the server...");

    // if were not shutdown, we need to try to reconnect
    if (!shutdown_) {
      connect();
    }
  }

 protected:
  bool shutdown_ = false;
  bool connected_ = false;
  std::string address_;

  std::mutex mutex_;
  std::condition_variable cv_;

  std::unique_ptr<ClientContext> connect_ctx_ = nullptr;
  std::unique_ptr<ConnStream> connect_stream_ = nullptr;
  std::unique_ptr<SvcStub> stub_ptr_ = nullptr;
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_CLIENT_HPP_
