#ifndef ABEILLE_RPC_CLIENT_HPP_
#define ABEILLE_RPC_CLIENT_HPP_

#include <grpcpp/grpcpp.h>

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "utils/include/errors.hpp"
#include "utils/include/logger.hpp"

using grpc::ClientContext;

namespace abeille {
namespace rpc {

template <typename ConnReq, typename ConnResp, typename Svc>
class Client {
 public:
  using ConnStream = grpc::ClientReaderWriter<ConnReq, ConnResp>;
  using SvcStub = typename Svc::Stub;

  Client() = default;
  explicit Client(const std::vector<std::string> &cluster_addresses) noexcept
      : cluster_addresses_(cluster_addresses) {}
  ~Client() = default;

  void SetClusterAddresses(const std::vector<std::string> &cluster_addresses) {
    cluster_addresses_ = cluster_addresses;
  }

  virtual void CommandHandler(const ConnResp &resp) = 0;

  virtual void StatusHandler(ConnReq &req) = 0;

  error Run() {
    address_ = cluster_addresses_[0];
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
  void connect() {
    {
      // try to connect to the server; last stream and context are preserved
      std::lock_guard<std::mutex> lk(mutex_);

      createStub();
      size_t index = 0;
      while (!handshake() && !shutdown_) {
        LOG_ERROR("failed to connect to [%s], trying another node...",
                  address_.c_str());

        index = (index + 1) % cluster_addresses_.size();
        address_ = cluster_addresses_[index];
        createStub();

        std::this_thread::sleep_for(std::chrono::seconds(3));
      }

      // if were shutdown, notify and return to perform shutdown
      if (shutdown_) {
        cv_.notify_one();
        return;
      }

      LOG_INFO("successfully connected to [%s]", address_.c_str());
      connected_ = true;
    }
    cv_.notify_one();

    keepAlive();
  }

  void createStub() {
    auto channel =
        grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());
    stub_ptr_ = Svc::NewStub(channel);
  }

  bool handshake() {
    ConnReq req;
    connect_ctx_ = std::make_unique<ClientContext>();
    connect_stream_ = stub_ptr_->Connect(connect_ctx_.get());
    return connect_stream_->Write(req);
  }

  void keepAlive() {
    ConnResp resp;
    while (connect_stream_->Read(&resp) && !shutdown_) {
      CommandHandler(resp);

      ConnReq req;
      StatusHandler(req);

      connect_stream_->Write(req);
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
  std::vector<std::string> cluster_addresses_;

  std::mutex mutex_;
  std::condition_variable cv_;

  std::unique_ptr<ClientContext> connect_ctx_ = nullptr;
  std::unique_ptr<ConnStream> connect_stream_ = nullptr;
  std::unique_ptr<SvcStub> stub_ptr_ = nullptr;
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_CLIENT_HPP_
