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

  ClientContext ctx;
  // TODO:
  // ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

  Empty req, resp;
  connect_stream_ptr_ = stub_ptr_->Connect(&ctx);

  bool ok = connect_stream_ptr_->Write(req);
  if (!ok) {
    return error("failed to write to the stream");
  }

  ok = connect_stream_ptr_->Read(&resp);
  if (!ok) {
    return error("failed to read from the stream");
  }

  connect_thread_ = std::make_unique<std::thread>(std::thread(&Client::connect, this));

  return error();
}

void Client::Shutdown() {
  shutdown_ = true;
  if (connect_thread_->joinable()) {
    connect_thread_->join();
  }
}

void Client::createStub() {
  auto channel = grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());
  stub_ptr_ = WorkerService::NewStub(channel);
}

void Client::connect() {
  Empty req, resp;
  while (connect_stream_ptr_->Read(&resp) && !shutdown_) {
    LOG_TRACE();
    connect_stream_ptr_->Write(req);
  }
}

}  // namespace worker
}  // namespace abeille
