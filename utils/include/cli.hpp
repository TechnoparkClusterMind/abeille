#ifndef CLI_HPP_
#define CLI_HPP_

#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "convert.hpp"

namespace abeille {
namespace cli {

class CLI {
 public:
  using args_type = const std::vector<std::string> &;
  using handler = std::function<std::string(args_type)>;
  using handlers_map = std::unordered_map<std::string, handler>;
  using handlers_helper = std::vector<std::pair<std::string, std::string>>;

  CLI(const handlers_map &handlers, const handlers_helper &helper) noexcept;

  void Process(std::string &line) const;

  void Help() const noexcept;

 private:
  handlers_map handlers_;
  handlers_helper helper_;
};

}  // namespace cli
}  // namespace abeille

#endif  // CLI_HPP_
