#ifndef ABEILLE_USER_COMMANDS_HPP_
#define ABEILLE_USER_COMMANDS_HPP_

#include "rpc/proto/abeille.grpc.pb.h"
#include "user_client.hpp"
#include "utils/include/cli.hpp"
#include "utils/include/constants.hpp"

using abeille::cli::CLI;
using abeille::user::Client;

namespace abeille {
namespace user {
namespace commands {

struct Wrapper {
  Wrapper() noexcept;
  void RunUserClient();

  CLI::handlers_map handlers;
  CLI::handlers_helper helper;

  // FIXME: make a way to pass here all addresses to raft nodes
  inline static Client user_client = Client(abeille::USER_SERVICE_ADDRESS);
};

std::string UploadData(CLI::args_type args);

std::string Status(CLI::args_type args);

}  // namespace commands
}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_COMMANDS_HPP_
