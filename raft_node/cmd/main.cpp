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

bool shutdown = false;
std::unique_ptr<Core> core_ptr = nullptr;
// Config will be refactored to signleton
Config config;

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

void parse_command_line_arguments(int argc, char** argv) {
  config.init();
}

void run() {
  if (config.is_ok()) {
    LOG_DEBUG("Initializing core...");
    core_ptr = std::make_unique<Core>(std::move(config));

    LOG_DEBUG("Running core...");
    core_ptr->Run();

  } else {
    LOG_ERROR("Config is not fully initialized. Exiting ...");
  }
}

int main(int argc, char *argv[]) {
  std::signal(SIGINT, signal_handler);
  parse_command_line_arguments(argc, argv);
  // logic for parsing of the config file

  run();

  auto shutdown_thread = std::thread(&listen_shutdown);
  shutdown_thread.join();

  return 0;
}
