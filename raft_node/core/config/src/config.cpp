#include "config.hpp"

#include "constants.hpp"

namespace abeille {

void Config::init() noexcept {
  id_ = 127;
  user_address_ = USER_SERVICE_ADDRESS;
  raft_address_ = RAFT_SERVICE_ADDRESS;
  worker_address_ = WORKER_SERVICE_ADDRESS;
}

}  // namespace abeille
