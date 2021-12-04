#include "core.hpp"

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

#include "config.hpp"
#include "logger.hpp"
#include "raft_service.hpp"
#include "user_service.hpp"
#include "worker_service.hpp"

namespace abeille {
namespace raft_node {

// Initializing all main objects of the raft_node
Core::Core(Config &&conf) noexcept
    : server_id_(config_.GetId()),
      config_(std::move(conf))
// raft_pool_(new RaftPool{raft_}), worker_service_(new WorkerService{task_mgr_}),
// raft_(new RaftConsensus{this}), task_mgr_(new TaskManager{this}),
// raft_service_(new RaftServiceImpl(raft_)),
// user_service_(new UserServiceImpl(task_mgr_))
{
  raft_ = std::make_shared<RaftConsensus>(this);
  task_mgr_ = std::make_shared<TaskManager>(this);

  raft_pool_ = std::make_shared<RaftPool>(raft_);

  raft_service_ = std::make_unique<RaftServiceImpl>(raft_);
  user_service_ = std::make_unique<UserServiceImpl>(task_mgr_);
  worker_service_ = std::make_unique<WorkerServiceImpl>();

  std::vector<std::string> raft_addr = {config_.GetRaftAddress(), config_.GetUserAddress(), config_.GetWorkerAddress()};
  std::vector<grpc::Service *> raft_vec = {raft_service_.get(), user_service_.get(), worker_service_.get()};
  raft_server_ = std::make_unique<abeille::rpc::Server>(raft_addr, raft_vec);
}

Core::Core(Core &&other) noexcept
    : raft_(std::move(other.raft_)),
      task_mgr_(std::move(other.task_mgr_)),
      raft_pool_(std::move(other.raft_pool_)),
      raft_service_(std::move(other.raft_service_)),
      user_service_(std::move(other.user_service_)),
      raft_server_(std::move(other.raft_server_)) {}

// TODO: Test for EXPECT_DEATH
void Core::Run() {
  LOG_INFO("Launching top-level objects");
  // FIXME: not fully implemented
  raft_->Run();
  task_mgr_->Run();
  raft_pool_->Run();
  raft_server_->Run();
}

void Core::Shutdown() {
  if (!shutdown_) {
    shutdown_ = true;
    LOG_INFO("Shutting down all instances...");

    // FIXME: not fully implemented
    raft_->Shutdown();
    task_mgr_->Shutdown();
    raft_pool_->Shutdown();
    raft_server_->Shutdown();

    LOG_INFO("Waiting for threads to finish...");
    std::unique_lock<std::mutex> lock_guard(mutex);
    while (num_workers_threads_ > 0) state_changed_.wait(lock_guard);
    while (num_peers_threads_ > 0) state_changed_.wait(lock_guard);
    LOG_INFO("Threads finished");
  }
}

Core::~Core() { Shutdown(); }

}  // namespace raft_node
}  // namespace abeille
