#include <csignal>
#include <iostream>

#include "constants.hpp"
#include "server.hpp"
#include "user_service.hpp"

using abeille::rpc::Server;

void signal_handler(int signal) {}

int main() {
  std::signal(SIGINT, signal_handler);

  std::vector<std::string> hosts = {USER_SERVICE_HOST};

  rpc::UserServiceImpl user_service;
  std::vector<grpc::Service*> services = {&user_service};

  Server server(hosts, services);

  error err = server.Run();
  if (!err.ok()) {
    server.Shutdown();
  } else {
    server.Wait();
  }

  return 0;
}
