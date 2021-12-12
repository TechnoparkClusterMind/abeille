#ifndef ABEILLE_RAFT_TYPES_H_
#define ABEILLE_RAFT_TYPES_H_

#include <cstdint>
#include <string>

namespace abeille {

using Index = uint64_t;
using Term = uint64_t;
using ServerId = uint64_t;
using Bytes = std::string;

}  // namespace abeille

#endif  // ABEILLE_RAFT_COMMON_H_
