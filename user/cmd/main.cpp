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
#include "user_client.hpp"

using abeille::cli::CLI;
using abeille::user::Client;

// ud user/data/test.json

// FIXME: make a way to pass here all addresses to raft nodes
Client user_client({abeille::USER_SERVICE_ADDRESS});

std::string Exit(CLI::args_type) { throw std::exception(); }

std::string UploadData(CLI::args_type args) {
  if (args.size() != 2) {
    return "error: invalid usage";
  }

  std::string filepath = args[1];
  std::ifstream file(filepath);
  if (!file.good()) {
    LOG_TRACE();
    return "failed to open the file";
  }

  std::stringstream input;
  input << file.rdbuf();

  auto task = new TaskData();
  auto status = google::protobuf::util::JsonStringToMessage(input.str(), task);
  if (!status.ok()) {
    return status.message().ToString();
  }

  auto response = user_client.UploadData(task);

  std::stringstream output;
  output << "task id is [" << response.task_id() << "], assigned to[" << uint2address(response.task_id()) << "]";

  return output.str();
}

void RunCLI() {
  const CLI::handlers_map handlers = {{"exit", Exit}, {"ud", UploadData}};

  const CLI::handlers_helper helper = {
      {"exit", "gracefully terminates the programm"},
      {"ud <filepath>", "uploads data from the <filepath> to abeille (json format with accordance to the proto file)"},
  };

  CLI cli(handlers, helper);

  linenoise::SetMultiLine(true);
  linenoise::SetHistoryMaxLen(4);

  std::string line;
  while (true) {
    auto quit = linenoise::Readline("abeille> ", line);
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
