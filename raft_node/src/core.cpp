#include <memory>

#include "core.hpp"
#include "config.hpp"
#include "raft_service.hpp"
#include "user_service.hpp"

namespace abeille {
namespace raft_node {

// Initializing all main objects of the raft_node
Core::Core(Config &conf)
    : config_(std::move(conf)), server_id_(config_.GetCurrentServerId()),
      raft_pool_(new RaftPool(this)),
      worker_pool_(new WorkerPool(this)),
      raft_(new RaftConsensus(this)),
      task_mgr_(new TaskManager(this)),
      raft_server_(config_.GetRaftAdress(),
                   std::make_unique<rpc::RaftServiceImpl>(raft_)),
      user_server_(config_.GetUserAddress(),
                   std::make_unique<rpc::UserServiceImpl>(raft_)),
      num_peers_thread_(0),
      num_workers_thread_(0),
      mutex() {}

// TODO: Test for EXPECT_DEATH
void Core::Run() {
  raft_server_->Run();
  user_server_->Run();
  raft_pool_->Run();
  worker_pool_->Run();
  raft_->Run();
  task_mgr_->Run();
}

void Core::shutdown() {
  user_server_->Shutdown();
  task_mgr_->Shutdown();
  raft_->Shutdown();
  worker_pool_->Shutdown();
  raft_pool_->Shutdown();
  raft_server_->Shutdown();
}

// No manual way to stop the daemon's work, only destructor
// TODO:
  Core::~Core() {
    if (!exiting)
      shutdown();

    std::unique_lock<std::mutex> lock_guard(mutex);
    while(num_workers_thread_ > 0)
      state_changed_.wait(lock_guard);
    while(num_peers_thread_ > 0)
      state_changed_.wait(lock_guard);
  }

// TODO:
void Core::HandleSignal(int signum) {}

} // namespace raft_node
} // namespace abeille
