#include "utils/include/cli.hpp"

namespace abeille {
namespace cli {

CLI::CLI(const handlers_map &handlers, const handlers_helper &helper) noexcept
    : handlers_(handlers), helper_(helper) {
  helper_.push_back({HELP_COMMAND, "lists available functions"});
  helper_.push_back({EXIT_COMMAND, "terminates the programm"});
}

bool CLI::Process(std::string &line) {
  args_type args = Split(line, ' ');
  std::string command = args[0];
  if (command == HELP_COMMAND) {
    Help();
  } else if (command == EXIT_COMMAND) {
    Exit();
  } else {
    auto it = handlers_.find(command);
    if (it != handlers_.end()) {
      std::cout << it->second(args) << std::endl;
    } else {
      std::cout << "cli: command not found: " << command << std::endl;
    }
  }
  return exit_;
}

void CLI::Help() const noexcept {
  for (const auto &value : helper_) {
    std::cout << "-- " << std::left << std::setw(50) << value.first
              << value.second << std::endl;
  }
}

void CLI::Exit() noexcept { exit_ = true; }

}  // namespace cli
}  // namespace abeille