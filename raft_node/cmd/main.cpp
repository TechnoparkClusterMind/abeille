#include <csignal>
#include <iostream>
#include <memory>
#include <thread>

#include "config.hpp"
#include "core.hpp"
#include "logger.hpp"

// FIXME: some threads don't get joined

using abeille::Config;
using abeille::raft_node::Core;
using namespace std::chrono_literals;

bool shutdown = false;
std::unique_ptr<Core> core_ptr = nullptr;

void signal_handler(int signal) { shutdown = true; }

void listen_shutdown() {
  while (true) {
    if (shutdown) {
      if (core_ptr) {
        core_ptr->Shutdown();
      }
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

void run() {
  LOG_DEBUG("Initializing config...");
  Config conf;
  conf.init();

  LOG_DEBUG("Initializing core...");
  core_ptr = std::make_unique<Core>(std::move(conf));

  LOG_DEBUG("Running core...");
  core_ptr->Run();
}

int main(int argc, char *argv[]) {
  std::signal(SIGINT, signal_handler);

  run();

  auto shutdown_thread = std::thread(&listen_shutdown);
  shutdown_thread.join();

  return 0;
}
