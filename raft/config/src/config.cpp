#include "raft/config/include/config.hpp"

#include <google/protobuf/util/json_util.h>

#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

using namespace google::protobuf;
using namespace google::protobuf::util;

namespace abeille {

bool Config::is_ok() const noexcept {
  return id_ != 0 && !user_address_.empty() && !raft_address_.empty() &&
         !worker_address_.empty();
}

void Config::parse(std::ifstream &file) {
  util::JsonParseOptions opt;
  opt.ignore_unknown_fields = true;
  opt.case_insensitive_enum_parsing = false;

  std::string file_data = std::string(std::istream_iterator<char>(file),
                                      std::istream_iterator<char>());

  RaftConfig proto_config;
  auto status = JsonStringToMessage(file_data, &proto_config, opt);
  if (!status.ok()) {
    LOG_ERROR(status.message().ToString().c_str());
    return;
  }

  user_address_ = proto_config.user_address();
  raft_address_ = proto_config.raft_address();
  worker_address_ = proto_config.worker_address();

  id_ = address2uint(raft_address_);

  auto peers = proto_config.peers();
  auto workers = proto_config.workers();
  peers_ = std::move(std::vector(peers.begin(), peers.end()));
  workers_ = std::move(std::vector(workers.begin(), workers.end()));
}

}  // namespace abeille
