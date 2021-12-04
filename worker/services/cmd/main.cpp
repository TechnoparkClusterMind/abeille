#include <grpcpp/grpcpp.h>

#include <vector>

#include "server.hpp"
#include "worker_service.hpp"

using abeille::WorkerServiceImpl;

int main() {
  std::vector<std::string> addr = {"127.0.0.1:9999"};
  auto worker_service_ = std::make_unique<WorkerServiceImpl>();
  std::vector<grpc::Service *> services = {worker_service_.get()};
  auto server = std::make_unique<abeille::rpc::Server>(addr, services);
  server->Run();
  server->Wait();
  return 0;
}
