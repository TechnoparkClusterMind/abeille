#include <csignal>
#include <iostream>

#include "constants.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "user_service.hpp"

using abeille::raft_node::UserServiceImpl;
using abeille::rpc::Server;

Server server;
bool shutdown = false;

void signal_handler(int signal) { shutdown = true; }

void listen_shutdown() {
  while (true) {
    if (shutdown) {
      server.Shutdown();
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

void run() {
  std::vector<std::string> hosts = {abeille::USER_SERVICE_HOST};

  UserServiceImpl user_service;
  std::vector<grpc::Service*> services = {&user_service};

  server = Server(hosts, services);

  error err = server.Run();
  if (!err.ok()) {
    server.Shutdown();
  } else {
    server.Wait();
  }
}

int main() {
  std::signal(SIGINT, signal_handler);
  std::thread(&listen_shutdown).detach();
  run();
  return 0;
}
