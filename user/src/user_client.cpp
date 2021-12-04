#include "user_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace user {

UploadDataResponse Client::UploadData(TaskData *task_data) {
  createStub(addresses_[address_index_]);
  connect();
  return uploadData(task_data);
}

void Client::createStub(const std::string &address) {
  auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
  stub_ptr_ = UserService::NewStub(channel);
}

void Client::connect() {
  while (true) {
    if (pingRemote()) {
      break;
    }
    LOG_ERROR("Failed to ping [%s]", addresses_[address_index_].c_str());
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

bool Client::pingRemote() {
  Empty req, resp;
  ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
  bool ok = stub_ptr_->Ping(&context, req, &resp).ok();
  return ok;
}

UploadDataResponse Client::uploadData(TaskData *task_data) {
  while (true) {
    ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

    UploadDataRequest request;
    request.set_allocated_task_data(task_data);

    UploadDataResponse response;

    Status status = stub_ptr_->UploadData(&context, request, &response);
    if (!status.ok()) {
      if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
        address_index_ = (address_index_ + 1) % addresses_.size();
        LOG_ERROR("Deadline exceeded. Trying server [%s]...", addresses_[address_index_].c_str());
      } else {
        LOG_ERROR("RPC failed: %d: %s", status.error_code(), status.error_message());
        LOG_INFO("Retrying...");
      }
      UploadData(task_data);
    }

    if (!response.success()) {
      std::string address = uint2address(response.leader_id());
      auto it = std::find(addresses_.begin(), addresses_.end(), address);
      if (it == addresses_.end()) {
        LOG_ERROR("Redirected to missing raft node [%s]", address.c_str());

        address_index_ = (address_index_ + 1) % addresses_.size();
        LOG_ERROR("Trying server [%s]...", addresses_[address_index_].c_str());

        UploadData(task_data);
      } else {
        address_index_ = std::distance(addresses_.begin(), it);
        LOG_INFO("Redirected to the leader [%s]...", addresses_[address_index_].c_str());
        UploadData(task_data);
      }
    } else {
      return response;
    }
  }
}

}  // namespace user
}  // namespace abeille
