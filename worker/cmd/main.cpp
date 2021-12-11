#include <chrono>
#include <csignal>
#include <string>
#include <thread>
#include <vector>

#include "common/include/client_config.hpp"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"
#include "worker/include/worker_client.hpp"

using abeille::common::ClientConfig;
using abeille::worker::Client;

bool shutdown = false;
std::unique_ptr<Client> worker_client;

void signal_handler(int signal) { shutdown = true; }

void run() {
  worker_client =
      std::make_unique<Client>(ClientConfig::Instance().cluster_addresses);

  error err = worker_client->Run();
  if (!err.ok()) {
    LOG_ERROR(err.what().c_str());
  }
}

void listen_shutdown() {
  while (true) {
    if (shutdown) {
      if (worker_client) {
        worker_client->Shutdown();
      }
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "expected [config path]" << std::endl;
    return -1;
  }

  std::string config_path = argv[1];
  error err = ClientConfig::Instance().Init(config_path);
  if (!err.ok()) {
    std::cout << err.what() << std::endl;
    return -1;
  }

  std::signal(SIGINT, signal_handler);
  auto shutdown_thread = std::thread(&listen_shutdown);
  run();
  shutdown_thread.join();

  return 0;
}
