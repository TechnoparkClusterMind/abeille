#include "user/include/user_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"

using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace user {

Client::Client(const std::vector<std::string> &addresses) noexcept
    : address_(addresses[0]), addresses_(addresses) {}

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
  auto channel =
      grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());
  stub_ptr_ = UserService::NewStub(channel);
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

// TODO: refactor it
void Client::keepAlive() {
  // keep the connection alive: respond to beats from the server
  UserConnectResponse response;
  while (connect_stream_->Read(&response) && !shutdown_) {
    if (response.command() == USER_COMMAND_REDIRECT) {
      connected_ = false;
      leader_id_ = response.leader_id();
      address_ = uint2address(response.leader_id());
      connect_stream_->WritesDone();
      LOG_INFO("got redirected to the [%s]", address_.c_str());
      Run();
    }

    if (response.command() == USER_COMMAND_RECIEVE_RESULT) {
      LOG_INFO("received a result");
    }

    UserConnectRequest request;
    request.set_status(status_);

    if (status_ == USER_STATUS_UPLOAD_DATA) {
      if (task_datas_.empty()) {
        LOG_ERROR("user status upload data, but empty task data queue");
      } else {
        request.set_allocated_task_data(task_datas_.front());
        task_datas_.pop();
      }
      status_ = USER_STATUS_NONE;
    }

    connect_stream_->Write(request);
  }

  connected_ = false;
  connect_stream_->WritesDone();
  LOG_INFO("disconnected from the server...");

  // if were not shutdown, we need to try to reconnect
  if (!shutdown_) {
    connect();
  }
}

bool Client::handshake() {
  UserConnectRequest request;
  request.set_status(status_);

  connect_ctx_ = std::make_unique<ClientContext>();
  connect_stream_ = stub_ptr_->Connect(connect_ctx_.get());

  return connect_stream_->Write(request);
}

void Client::UploadData(TaskData *task_data) {
  LOG_TRACE();
  status_ = USER_STATUS_UPLOAD_DATA;
  task_datas_.push(task_data);
}

// UploadDataResponse Client::uploadData(TaskData *task_data) {
//   UploadDataRequest request;
//   UploadDataResponse response;
//   request.set_allocated_task_data(task_data);

//   while (true) {
//     ClientContext context;
//     context.set_authority(abeille::USER_SERVICE_ADDRESS);
//     context.set_deadline(std::chrono::system_clock::now() +
//                          std::chrono::seconds(5));

//     Status status = stub_ptr_->UploadData(&context, request, &response);
//     if (!status.ok()) {
//       if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
//         address_index_ = (address_index_ + 1) % addresses_.size();
//         LOG_ERROR("Deadline exceeded. Trying server [%s]...",
//                   addresses_[address_index_].c_str());
//       } else {
//         LOG_ERROR("RPC failed: %d: %s", status.error_code(),
//                   status.error_message());
//         LOG_INFO("Retrying...");
//       }
//       UploadData(task_data);
//     }

//     if (!response.success()) {
//       std::string address = uint2address(response.leader_id());
//       auto it = std::find(addresses_.begin(), addresses_.end(), address);
//       if (it == addresses_.end()) {
//         LOG_ERROR("Redirected to missing raft node [%s]", address.c_str());

//         address_index_ = (address_index_ + 1) % addresses_.size();
//         LOG_ERROR("Trying server [%s]...",
//         addresses_[address_index_].c_str());
//       } else {
//         address_index_ = std::distance(addresses_.begin(), it);
//         LOG_INFO("Redirected to the leader [%s]...",
//                  addresses_[address_index_].c_str());
//       }
//       UploadData(task_data);
//     }

//     LOG_DEBUG("successfully uploaded data, returning response...");
//     return response;
//   }
// }

}  // namespace user
}  // namespace abeille
