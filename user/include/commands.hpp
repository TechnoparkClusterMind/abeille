#ifndef ABEILLE_USER_COMMANDS_HPP_
#define ABEILLE_USER_COMMANDS_HPP_

#include "common/include/client_config.hpp"
#include "user_client.hpp"
#include "utils/include/cli.hpp"

using abeille::cli::CLI;
using abeille::common::ClientConfig;
using abeille::user::Client;

namespace abeille {
namespace user {
namespace commands {

struct Wrapper {
  Wrapper() noexcept;
  void RunUserClient();

  CLI::handlers_map handlers;
  CLI::handlers_helper helper;

  inline static Client user_client;
};

std::string UploadData(CLI::args_type args);

std::string Status(CLI::args_type args);

}  // namespace commands
}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_COMMANDS_HPP_
