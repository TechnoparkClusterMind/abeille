#include "worker_service.hpp"

#include <thread>

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {
namespace worker {

Status WorkerServiceImpl::Connect(ServerContext *context, ConnectStream *stream) {
  std::string address = ExtractAddress(context->peer());
  LOG_INFO("connection request from [%s]", address.c_str());

  Empty response;
  WorkerStatus request;
  bool ok = stream->Read(&request);
  if (!ok) {
    LOG_ERROR("failed to read from the stream with [%s]", address.c_str());
    return Status::CANCELLED;
  }

  uint64_t id = address2uint(address);
  while (true) {
    ok = stream->Write(response);
    if (!ok) {
      LOG_WARN("connection with [%s] was lost", address.c_str());
      break;
    }

    ok = stream->Read(&request);
    if (!ok) {
      LOG_WARN("connection with [%s] was lost", address.c_str());
      break;
    }

    LOG_DEBUG("[%s] is [%d]", address.c_str(), request.status());

    workers_[id] = request.status();
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  return Status::OK;
}

}  // namespace worker
}  // namespace abeille
