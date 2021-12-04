#include <string>
#include <vector>

#include "convert.hpp"
#include "logger.hpp"
#include "worker_client.hpp"

using abeille::worker::Client;

int main() {
  std::string address = "127.0.0.1:9999";
  Client worker_client(address);

  error err = worker_client.Run();
  if (!err.ok()) {
    LOG_ERROR(err.what().c_str());
  }

  return 0;
}
