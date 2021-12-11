#ifndef ABEILLE_USER_CONFIG_HPP_
#define ABEILLE_USER_CONFIG_HPP_

#include <string>
#include <vector>

#include "utils/include/errors.hpp"

namespace abeille {
namespace common {

struct ClientConfig {
  static ClientConfig &Instance() {
    static ClientConfig instance;
    return instance;
  }

  error Init(const std::string &config_path);

  std::vector<std::string> cluster_addresses;
};

}  // namespace common
}  // namespace abeille

#endif  // ABEILLE_USER_CONFIG_HPP_
