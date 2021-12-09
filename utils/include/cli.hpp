#ifndef CLI_HPP_
#define CLI_HPP_

#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "convert.hpp"

namespace abeille {
namespace cli {

static constexpr const char *HELP_COMMAND = "help";
static constexpr const char *EXIT_COMMAND = "exit";

class CLI {
 public:
  using args_type = const std::vector<std::string> &;
  using handler = std::function<std::string(args_type)>;
  using handlers_map = std::unordered_map<std::string, handler>;
  using handlers_helper = std::vector<std::pair<std::string, std::string>>;

  CLI(const handlers_map &handlers, const handlers_helper &helper) noexcept;

  bool Process(std::string &line);

  virtual void Help() const noexcept;

  virtual void Exit() noexcept;

 private:
  bool exit_ = false;
  handlers_map handlers_;
  handlers_helper helper_;
};

}  // namespace cli
}  // namespace abeille

#endif  // CLI_HPP_
