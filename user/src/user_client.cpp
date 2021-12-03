#include "user_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {

UploadDataResponse user::Client::UploadData(TaskData *task_data) {
  createStub();
  connect();
  return uploadData(task_data);
}

void user::Client::createStub() {
  auto channel = grpc::CreateChannel(host_, grpc::InsecureChannelCredentials());
  stub_ptr_ = UserService::NewStub(channel);
}

void user::Client::connect() {
  while (true) {
    if (pingRemote()) {
      break;
    }
    LOG_ERROR("Failed to ping remote");
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

bool user::Client::pingRemote() {
  Empty req, resp;
  ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
  bool ok = stub_ptr_->Ping(&context, req, &resp).ok();

  return ok;
}

UploadDataResponse user::Client::uploadData(TaskData *task_data) {
  UploadDataRequest request;
  request.set_allocated_task_data(task_data);

  ClientContext context;
  UploadDataResponse response;
  Status status = stub_ptr_->UploadData(&context, request, &response);
  if (!status.ok()) {
    LOG_ERROR("RPC failed: %d: %s", status.error_code(), status.error_message());
  } else {
    LOG_INFO("Response: %u", response.task_id());
  }

  return response;
}

}  // namespace abeille
