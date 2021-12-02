#ifndef ABEILLE_CONSTANTS_H_
#define ABEILLE_CONSTANTS_H_

#include <cstdint>

namespace abeille {

// FIXME: have to be implemented in Config !!!
static constexpr char USER_CLIENT_HOST[] = "localhost:50051";
static constexpr char USER_SERVICE_HOST[] = "localhost:50051";

static constexpr char RAFT_SERVICE_HOST[] = "localhost:50052";

}  // namespace abeille

#endif  // ABEILLE_CONSTANTS_H_
