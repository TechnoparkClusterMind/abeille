#include <google/protobuf/util/json_util.h>

#include <csignal>
#include <memory>
#include <string>
#include <vector>

#include "common/include/client_config.hpp"
#include "linenoise.hpp"
#include "user/include/commands.hpp"
#include "user/include/user_client.hpp"
#include "utils/include/cli.hpp"

using abeille::cli::CLI;
using abeille::common::ClientConfig;
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
  if (argc != 3) {
    std::cout << "expected [config path] and [data dir path]" << std::endl;
    return -1;
  }

  std::string config_path = argv[1];
  error err = ClientConfig::Instance().Init(config_path);
  if (!err.ok()) {
    std::cout << err.what() << std::endl;
    return -1;
  }

  wrapper.user_client.SetClusterAddresses(
      ClientConfig::Instance().cluster_addresses);

  std::signal(SIGINT, SignalHandler);
  std::thread(ListenShutdown).detach();

  std::thread([] {
    wrapper.RunUserClient();
    RunCLI();
  }).join();

  return 0;
}
