#include "common/include/client_config.hpp"

#include <google/protobuf/util/json_util.h>

#include <fstream>
#include <sstream>

#include "rpc/proto/abeille.pb.h"

namespace abeille {
namespace common {

using google::protobuf::util::JsonStringToMessage;

error ClientConfig::Init(const std::string &config_path) {
  std::ifstream input(config_path);
  if (!input.good()) {
    return error("failed to open the config path");
  }

  std::stringstream body;
  body << input.rdbuf();

  auto user_config = std::make_unique<UserConfig>();
  auto status = JsonStringToMessage(body.str(), user_config.get());
  if (!status.ok()) {
    return error(status.message().as_string());
  }

  if (user_config->cluster_addresses_size() == 0) {
    return error("no cluster addresses were given");
  }

  auto begin = user_config->cluster_addresses().begin();
  auto end = user_config->cluster_addresses().end();
  cluster_addresses = std::vector<std::string>(begin, end);

  return error();
}

}  // namespace common
}  // namespace abeille
