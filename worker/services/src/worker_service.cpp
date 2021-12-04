#include "worker_service.hpp"

#include "convert.hpp"
#include "logger.hpp"

namespace abeille {

Status WorkerServiceImpl::Connect(ServerContext *context, ServerReaderWriter<Empty, Empty> *stream) {
  LOG_INFO("Connection request from [%s]", ExtractAddress(context->peer()).c_str());
  return Status::OK;
}

}  // namespace abeille
