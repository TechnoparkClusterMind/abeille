#include "worker_client.hpp"

#include <chrono>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"
#include "user/include/data_processor.hpp"

using abeille::user::ProcessData;
using grpc::ClientContext;
using grpc::Status;

namespace abeille {
namespace worker {

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
  stub_ptr_ = WorkerService::NewStub(channel);
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
  WorkerConnectResponse response;
  while (connect_stream_->Read(&response) && !shutdown_) {
    LOG_DEBUG("command is [%s]",
              WorkerCommand_Name(response.command()).c_str());
    if (response.command() == WORKER_COMMAND_REDIRECT) {
      connected_ = false;
      leader_id_ = response.leader_id();
      address_ = uint2address(response.leader_id());
      connect_stream_->WritesDone();
      LOG_INFO("got redirected to the [%s]", address_.c_str());
      Run();
    }

    if (response.command() == WORKER_COMMAND_ASSIGN) {
      if (response.task_id() == 0) {
        LOG_ERROR("got assigned zero task");
      } else {
        LOG_INFO("got assigned [%llu] task", response.task_id());
        task_id_ = response.task_id();
        status_ = WORKER_STATUS_BUSY;
      }
    }

    if (response.command() == WORKER_COMMAND_PROCESS) {
      if (!response.has_task_data()) {
        LOG_ERROR("got asked to process null data");
      } else {
        std::thread(&Client::processData, this, response.task_data()).detach();
      }
    }

    WorkerConnectRequest request;
    request.set_status(status_);

    // if we have completed processing the data, switch the status and send the
    // result
    if (status_ == WORKER_STATUS_COMPLETED) {
      status_ = WORKER_STATUS_IDLE;
      request.set_task_id(task_id_);
      request.set_allocated_task_result(task_result_);
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
  WorkerConnectRequest request;
  request.set_status(status_);

  connect_ctx_ = std::make_unique<ClientContext>();
  connect_stream_ = stub_ptr_->Connect(connect_ctx_.get());

  return connect_stream_->Write(request);
}

void Client::processData(const TaskData &task_data) {
  LOG_INFO("processing data...");
  task_result_ = new TaskResult();
  ProcessData(task_data, task_result_);
  LOG_INFO("finished processing data");
  status_ = WORKER_STATUS_COMPLETED;
}

}  // namespace worker
}  // namespace abeille
