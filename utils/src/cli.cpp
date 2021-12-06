#include "cli.hpp"

namespace abeille {
namespace cli {

CLI::CLI(const handlers_map &handlers, const handlers_helper &helper) noexcept : handlers_(handlers), helper_(helper) {
  helper_.push_back({"help", "lists available functions"});
}

void CLI::Process(std::string &line) const {
  args_type args = Split(line, ' ');
  if (args[0] == "help") {
    Help();
  } else {
    auto it = handlers_.find(args[0]);
    if (it != handlers_.end()) {
      std::cout << it->second(args) << std::endl;
    } else {
      std::cout << "cli: command not found: " << args[0] << std::endl;
    }
  }
}

void CLI::Help() const noexcept {
  for (const auto &value : helper_) {
    std::cout << "-- " << std::left << std::setw(50) << value.first << value.second << std::endl;
  }
}

}  // namespace cli
}  // namespace abeille