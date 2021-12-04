#include <grpcpp/grpcpp.h>

#include <csignal>
#include <vector>

#include "logger.hpp"
#include "server.hpp"
#include "worker_service.hpp"

using abeille::rpc::Server;
using abeille::worker::WorkerServiceImpl;

bool shutdown = false;
std::unique_ptr<Server> server_ptr = nullptr;
std::unique_ptr<grpc::Service> worker_service = nullptr;

void signal_handler(int signal) { shutdown = true; }

void run() {
  std::vector<std::string> addr = {"127.0.0.1:9999"};

  worker_service = std::make_unique<WorkerServiceImpl>();
  std::vector<grpc::Service *> services = {worker_service.get()};

  server_ptr = std::make_unique<Server>(addr, services);
  error err = server_ptr->Run();
  if (!err.ok()) {
    LOG_ERROR(err.what().c_str());
    shutdown = true;
  }
}

void listen_shutdown() {
  while (true) {
    if (shutdown) {
      if (server_ptr) {
        server_ptr->Shutdown();
      }
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

int main() {
  std::signal(SIGINT, signal_handler);

  run();

  auto shutdown_thread = std::thread(&listen_shutdown);
  shutdown_thread.join();

  return 0;
}
