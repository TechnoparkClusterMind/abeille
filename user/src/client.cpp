#include "client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"

using grpc::ClientContext;
using grpc::Status;

void User::Client::StreamData(const std::vector<std::vector<int>>& arr) {
  connect();
  streamData(arr);
}

void User::Client::connect() {
  while (true) {
    if (pingRemote()) {
      break;
    }
    std::cout << "Failed to ping remote" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

bool User::Client::pingRemote() {
  createStub();

  Empty req, resp;
  ClientContext ctx;
  ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
  bool ok = stub_ptr_->Ping(&ctx, req, &resp).ok();

  return ok;
}

void User::Client::createStub() {
  if (stub_ptr_ == nullptr) {
    auto channel = grpc::CreateChannel(host_, grpc::InsecureChannelCredentials());
    stub_ptr_ = UserService::NewStub(channel);
  }
}

void User::Client::streamData(const std::vector<std::vector<int>>& arr) {
  ClientContext ctx;
  auto stream = stub_ptr_->StreamData(&ctx);

  for (size_t i = 0; i < arr.size(); ++i) {
    UserRequest user_request;
    user_request.set_task_id(i);
    for (int value : arr[i]) {
      user_request.add_data(value);
    }
    stream->Write(user_request);

    UserResponse user_response;
    stream->Read(&user_response);
    std::cout << user_response.result() << std::endl;
  }
  stream->WritesDone();

  Status status = stream->Finish();
  if (!status.ok()) {
    std::cout << "RPC failed: " << status.error_code() << ": " << status.error_message() << std::endl;
  }
}
