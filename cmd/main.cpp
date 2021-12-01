#include <csignal>
#include <iostream>

#include "constants.hpp"
#include "server.hpp"
#include "user_service.hpp"

using abeille::rpc::Server;
using abeille::rpc::UserServiceImpl;

Server server;

void signal_handler(int signal) {
  std::cout << "Exiting..." << std::endl;
  server.Shutdown();
}

int main() {
  std::signal(SIGINT, signal_handler);

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

  return 0;
}
