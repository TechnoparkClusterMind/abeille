#include <google/protobuf/util/json_util.h>

#include <csignal>
#include <memory>
#include <string>
#include <vector>

#include "cli.hpp"
#include "constants.hpp"
#include "convert.hpp"
#include "linenoise.hpp"
#include "logger.hpp"
#include "user/include/commands.hpp"
#include "user/include/user_client.hpp"

using abeille::cli::CLI;
using abeille::user::Client;

// ud user/data/test.json

bool shutdown = false;
abeille::user::commands::Wrapper wrapper;

void Shutdown() { wrapper.user_client.Shutdown(); }

void RunCLI() {
  CLI cli(wrapper.handlers, wrapper.helper);

  std::string line;
  while (!shutdown && !linenoise::Readline("abeille> ", line)) {
    bool exit = cli.Process(line);
    if (exit) {
      break;
    }
    linenoise::AddHistory(line.c_str());
  }
  Shutdown();
}

void SignalHandler(int signal) { shutdown = true; }

void ListenShutdown() {
  while (!shutdown) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  Shutdown();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    LOG_ERROR("expected path to task results directory");
    return -1;
  }

  std::signal(SIGINT, SignalHandler);
  std::thread(ListenShutdown).detach();

  std::thread([] {
    wrapper.RunUserClient();
    RunCLI();
  }).join();

  return 0;
}
