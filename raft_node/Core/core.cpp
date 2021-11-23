#include "core.h"

#include "raft.h"

namespace Abeille {
namespace Raft_node {

// Initializing all main objects of the raft_node
// TODO: maybe custom deleters are needed
Core::Core(Config &conf)
    : config(std::move(conf)),
      server_id(config.get_current_server_id()),
      raft_client(new Raft_client(config.get_raft_nodes())),
      worker_client(new Worker_client(config.get_worker_nodes())),
      raft(new Raft_consensus(raft_client)),
      task_mgr(new Task_manager(raft_client, worker_client)),
      raft_service(new Raft_service(raft)),
      user_service(new User_service(task_mgr)),
      raft_server(config.get_raft_adress(), raft_service.get()),
      user_server(config.get_user_address(), user_service.get()) {}

// TODO: Test for EXPECT_DEATH
void Core::run() {
    raft_server->run();
    user_server->run();
    raft_client->run();
    worker_client->run();
    raft->run();
    task_mgr->run();
}

// No manual way to stop the daemon's work, only destructor
// TODO:
Core::~Core() {}

// TODO:
void Core::handle_signal(int signum) {}

}  // namespace Raft_node
}  // namespace Abeille
