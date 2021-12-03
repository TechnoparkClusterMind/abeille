#include "core.hpp"

#include <grpcpp/grpcpp.h>

#include <memory>
#include <vector>

#include "config.hpp"
#include "logger.hpp"
#include "raft_service.hpp"
#include "user_service.hpp"

namespace abeille {
namespace raft_node {

// Initializing all main objects of the raft_node
Core::Core(Config &&conf)
    : server_id_(config_.GetId()),
      config_(std::move(conf))
// raft_pool_(new RaftPool{raft_}), worker_pool_(new WorkerPool{task_mgr_}),
// raft_(new RaftConsensus{this}), task_mgr_(new TaskManager{this}),
// raft_service_(new RaftServiceImpl(raft_)),
// user_service_(new UserServiceImpl(task_mgr_))
{
  num_peers_threads_ = 0;
  num_workers_threads_ = 0;

  // raft_ = nullptr;
  // task_mgr_ = nullptr;

  raft_ = std::make_shared<RaftConsensus>(this);
  task_mgr_ = std::make_shared<TaskManager>(this);
  raft_pool_ = std::make_shared<RaftPool>(raft_);
  worker_pool_ = std::make_shared<WorkerPool>(task_mgr_);

  raft_service_ = std::make_unique<RaftServiceImpl>(raft_);
  user_service_ = std::make_unique<UserServiceImpl>(task_mgr_);

  std::vector<std::string> raft_addr = {config_.GetRaftAddress()};
  std::vector<grpc::Service *> raft_vec = {raft_service_.get()};

  std::vector<std::string> user_addr = {config_.GetUserAddress()};
  std::vector<grpc::Service *> user_vec = {user_service_.get()};

  raft_server_ = std::make_unique<abeille::rpc::Server>(raft_addr, raft_vec);
  user_server_ = std::make_unique<abeille::rpc::Server>(user_addr, user_vec);
}

// TODO: Test for EXPECT_DEATH
void Core::Run() {
  LOG_INFO("Launching top-level objects");
  raft_server_->Run();
  user_server_->Run();
  // FIXME: not fully implemented
  raft_pool_->Run();
  worker_pool_->Run();
  raft_->Run();
  task_mgr_->Run();
}

void Core::Shutdown() {
  user_server_->Shutdown();
  // FIXME: not fully implemented
  task_mgr_->Shutdown();
  raft_->Shutdown();
  worker_pool_->Shutdown();
  raft_pool_->Shutdown();
  raft_server_->Shutdown();
}

Core::~Core() {
  if (!exiting_) Shutdown();

  std::unique_lock<std::mutex> lock_guard(mutex);
  while (num_workers_threads_ > 0) state_changed_.wait(lock_guard);
  while (num_peers_threads_ > 0) state_changed_.wait(lock_guard);
}

void Core::HandleSignal(int signum) {
  LOG_INFO("Shutting down ...");
  Shutdown();
}

}  // namespace raft_node
}  // namespace abeille
