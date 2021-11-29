#ifndef ABEILLE_RAFT_WORKER_POOL_H_
#define ABEILLE_RAFT_WORKER_POOL_H_
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "abeille.pb.h"
#include "worker.hpp"
#include "raft_consensus.hpp"

namespace abeille {
namespace raft_node {
class WorkerPool {
public:
  WorkerPool();
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_WORKER_POOL_H_
