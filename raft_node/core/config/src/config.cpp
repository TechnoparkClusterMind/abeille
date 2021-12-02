#include "config.hpp"
#include "constants.hpp"

namespace abeille {

void Config::init() noexcept {
  id_ = 127;
  user_address_ = USER_SERVICE_HOST;
  raft_address_ = RAFT_SERVICE_HOST;
}

} // namespace abeille
