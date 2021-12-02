#include <csignal>
#include <iostream>
#include <thread>

#include "config.hpp"
#include "core.hpp"

using namespace abeille;
using namespace abeille::raft_node;
using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  Config conf;
  conf.init();

  Core core(std::move(conf));
  core.Run();

  while (true)
    std::this_thread::sleep_for(60s);

  return 0;
}
