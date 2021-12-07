#include "core.hpp"

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

#include "config.hpp"
#include "logger.hpp"
#include "raft_service.hpp"
#include "user_service.hpp"
#include "worker_service.hpp"

using abeille::rpc::ServiceInfo;

namespace abeille {
namespace raft_node {

Core::Core(Config &&conf) noexcept
    : server_id_(config_.GetId()),
      config_(std::move(conf)),
      raft_(new RaftConsensus(this)),
      task_mgr_(new TaskManager(this)),
      raft_pool_(new RaftPool(raft_)),
      raft_service_(new RaftServiceImpl(raft_)),
      user_service_(new UserServiceImpl(task_mgr_)),
      worker_service_(new WorkerServiceImpl()) {

  std::vector<ServiceInfo> services = {{config_.GetRaftAddress(), raft_service_.get()},
                                       {config_.GetUserAddress(), user_service_.get()},
                                       {config_.GetWorkerAddress(), worker_service_.get()}};

  raft_server_ = std::make_unique<abeille::rpc::Server>(services);

}

Core::Core(Core &&other) noexcept
    : raft_(std::move(other.raft_)),
      task_mgr_(std::move(other.task_mgr_)),
      raft_pool_(std::move(other.raft_pool_)),
      raft_service_(std::move(other.raft_service_)),
      user_service_(std::move(other.user_service_)),
      worker_service_(std::move(other.worker_service_)),
      raft_server_(std::move(other.raft_server_)) {}

// TODO: Test for EXPECT_DEATH
Core::Status Core::Run() noexcept {
  LOG_INFO("Launching top-level objects");

  std::vector<Status> status(5);
  status[0] = raft_->Run();
  status[1] = task_mgr_->Run();
  status[2] = raft_pool_->Run();
  status[3] = worker_pool_->Run();
  status[4] = raft_server_->Run();

  if (!std::all_of(status.cbegin(), status.cend(),
                   [](const Status &st) { return st.ok(); })) {
    LOG_ERROR("Unable to launch top-level objects\n");
    Shutdown();
  }
}

void Core::Shutdown() noexcept {
  if (!shutdown_) {
    shutdown_ = true;
    LOG_INFO("Shutting down all instances...");

    raft_->Shutdown();
    task_mgr_->Shutdown();
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

}  // namespace raft_node
}  // namespace abeille
