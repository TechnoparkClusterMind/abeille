#ifndef ABEILLE_CONSTANTS_H_
#define ABEILLE_CONSTANTS_H_

#include <cstdint>

namespace abeille {

// FIXME: have to be implemented in Config !!!
static constexpr const char USER_SERVICE_ADDRESS[] = "127.0.0.1:50051";

static constexpr const char RAFT_SERVICE_ADDRESS[] = "127.0.0.1:50052";

static constexpr const char WORKER_SERVICE_ADDRESS[] = "127.0.0.1:50053";

}  // namespace abeille

#endif  // ABEILLE_CONSTANTS_H_
