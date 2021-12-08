#include <google/protobuf/util/json_util.h>

#include <csignal>
#include <memory>
#include <string>
#include <vector>

#include "cli.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "convert.hpp"
#include "linenoise.hpp"
#include "logger.hpp"
#include "user_client.hpp"

using abeille::cli::CLI;
using abeille::user::Client;

// ud user/data/test.json

void RunCLI() {
  abeille::user::commands::Wrapper wrapper;
  CLI cli(wrapper.handlers, wrapper.helper);

  linenoise::SetMultiLine(true);
  linenoise::SetHistoryMaxLen(4);

  std::string line;
  while (true) {
    bool quit = linenoise::Readline("abeille> ", line);
    if (quit) {
      break;
    }

    try {
      cli.Process(line);
    } catch (const std::exception &e) {  // is thrown in Exit
      std::cout << "goodbye!" << std::endl;
      break;
    }

    linenoise::AddHistory(line.c_str());
  }
}

// TODO: add graceful shutdown

int main(int argc, char *argv[]) {
  if (argc != 2) {
    LOG_ERROR("expected path to task results directory");
    return -1;
  }

  RunCLI();

  return 0;
}
