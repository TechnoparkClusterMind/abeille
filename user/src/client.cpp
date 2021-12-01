#include "client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {

void user::Client::Upload(const std::vector<int> &arr) {
  connect();
  upload(arr);
}

void user::Client::connect() {
  while (true) {
    if (pingRemote()) {
      break;
    }
    std::cout << "Failed to ping remote" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

bool user::Client::pingRemote() {
  createStub();

  Empty req, resp;
  ClientContext ctx;
  ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
  bool ok = stub_ptr_->Ping(&ctx, req, &resp).ok();

  return ok;
}

void user::Client::createStub() {
  if (stub_ptr_ == nullptr) {
    auto channel = grpc::CreateChannel(host_, grpc::InsecureChannelCredentials());
    stub_ptr_ = UserService::NewStub(channel);
  }
}

void user::Client::upload(const std::vector<int> &data) {
  TaskData *task_data = new TaskData();
  for (int value : data) {
    task_data->add_data(value);
  }

  UploadRequest request;
  request.set_allocated_task_data(task_data);

  ClientContext ctx;
  UploadResponse response;
  Status status = stub_ptr_->Upload(&ctx, request, &response);
  if (!status.ok()) {
    std::cout << "RPC failed: " << status.error_code() << ": " << status.error_message() << std::endl;
  } else {
    std::cout << "Response: " << response.task_id() << std::endl;
  }
}

}  // namespace abeille
