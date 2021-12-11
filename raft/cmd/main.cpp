#include <unistd.h>

#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "raft/config/include/config.hpp"
#include "raft/core/include/core.hpp"
#include "utils/include/logger.hpp"

// FIXME: some threads don't get joined
using abeille::Config;
using abeille::raft::Core;

bool shutdown = false;
std::unique_ptr<Core> core_ptr = nullptr;
std::string raft_config_name;

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

void help() {}

void parse_command_line_arguments(int argc, char** argv) {
  opterr = 1;
  int c;

  while ((c = getopt(argc, argv, "hi:")) != -1) {
    switch (c) {
      case 'h':
        help();
        break;

      case 'i':
        raft_config_name = std::string(optarg);
        break;

      default:
        help();
        exit(-1);
    }
  }
}

void run() {
  // Config will be refactored to signleton
  if (raft_config_name.empty()) {
    LOG_ERROR("No config file was found");
    help();
    exit(1);
  }

  Config config;
  std::ifstream file(raft_config_name, std::ios::in);

  if (file.is_open()) {
    config.parse(file);
  } else {
    LOG_ERROR("Unable to open file %s", raft_config_name.c_str());
    exit(2);
  }

  file.close();

  if (config.is_ok()) {
    LOG_DEBUG("Initializing core...");
    core_ptr = std::make_unique<Core>(std::move(config));

    LOG_DEBUG("Running core...");
    core_ptr->Run();

  } else {
    LOG_ERROR("Config wasn't initialized. Exiting ...");
    exit(3);
  }
}

int main(int argc, char* argv[]) {
  std::signal(SIGINT, signal_handler);

  parse_command_line_arguments(argc, argv);
  run();

  auto shutdown_thread = std::thread(&listen_shutdown);
  shutdown_thread.join();

  return 0;
}
