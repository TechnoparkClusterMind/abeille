#include "core.hpp"
#include "raft_service.hpp"
#include "user_service.hpp"

namespace abeille {
namespace raft_node {

// Initializing all main objects of the raft_node
Core::Core(Config &conf)
    : config_(std::move(conf)), server_id_(config_.GetCurrentServerId()),
      raft_pool_(new RaftPool(config_.GetRaftNodes())),
      worker_pool_(new WorkerPool(config_.GetWorkerNodes())),
      raft_(new RaftConsensus()),
      task_mgr_(new TaskManager(raft_pool_, worker_pool_)),
      raft_server_(config_.GetRaftAdress(),
                   std::make_unique<rpc::RaftServiceImpl>(raft_)),
      user_server_(config_.GetUserAddress(),
                   std::make_unique<rpc::UserServiceImpl>(raft_)) {}

// TODO: Test for EXPECT_DEATH
void Core::Run() {
  raft_server_->Run();
  user_server_->Run();
  raft_pool_->Run();
  worker_pool_->Run();
  raft_->Run();
  task_mgr_->Run();
}

// No manual way to stop the daemon's work, only destructor
// TODO:
Core::~Core() {}

// TODO:
void Core::HandleSignal(int signum) {}

} // namespace raft_node
} // namespace abeille
