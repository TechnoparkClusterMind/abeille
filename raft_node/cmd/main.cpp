#include <csignal>
#include <iostream>
#include <thread>

#include "config.hpp"
#include "core.hpp"
#include "logger.hpp"

using namespace abeille;
using namespace abeille::raft_node;
using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  LOG_DEBUG("Initializing config...");
  Config conf;
  conf.init();

  LOG_DEBUG("Initializing core...");
  Core core(std::move(conf));
  LOG_DEBUG("Running core...");
  core.Run();

  while (true) {
    std::this_thread::sleep_for(60s);
  }

  return 0;
}
