#include "raft/core/include/core.hpp"

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

#include "raft/config/include/config.hpp"
#include "raft/rpc/include/raft_service.hpp"
#include "raft/rpc/include/user_service.hpp"
#include "raft/rpc/include/worker_service.hpp"
#include "utils/include/logger.hpp"

using abeille::rpc::ServiceInfo;

namespace abeille {
namespace raft {

Core::Core(Config &&conf) noexcept
    : server_id_(config_.GetId()),
      config_(std::move(conf)),
      raft_(new RaftConsensus(this)),
      task_mgr_(new TaskManager(this)),
      raft_pool_(new RaftPool(raft_)),
      raft_service_(new RaftServiceImpl(raft_)),
      user_service_(new UserServiceImpl(raft_, task_mgr_)),
      worker_service_(new WorkerServiceImpl(this)) {
  std::vector<ServiceInfo> services = {
      {config_.GetRaftAddress(), raft_service_.get()},
      {config_.GetUserAddress(), user_service_.get()},
      {config_.GetWorkerAddress(), worker_service_.get()}};

  raft_server_ = std::make_unique<abeille::rpc::Server>(services);
}

// TODO: Test for EXPECT_DEATH
void Core::Run() noexcept {
  LOG_INFO("Launching top-level objects");
  // Raft and pool are supposed to be earlier rather than server
  raft_->Run();
  raft_pool_->Run();

  Status server_status = raft_server_->Run();
  if (!server_status.ok()) {
    LOG_ERROR("Server wasn't started. Shutting down raft node ...");
    Shutdown();
  }
}

void Core::Shutdown() noexcept {
  if (!shutdown_) {
    shutdown_ = true;
    LOG_INFO("Shutting down all instances...");

    raft_->Shutdown();
    raft_pool_->Shutdown();
    raft_server_->Shutdown();

    LOG_INFO("Waiting for threads to finish...");
    pool_state_changed_.notify_all();
    std::unique_lock<std::mutex> lock_guard(mutex);
    while (num_workers_threads_ > 0 || num_workers_threads_ > 0)
      pool_state_changed_.wait(lock_guard);

    LOG_INFO("All threads have been shutted down");
  }
}

Core::~Core() { Shutdown(); }

}  // namespace raft
}  // namespace abeille
