#ifndef ABEILLE_RAFT_RAFT_POOL_H_
#define ABEILLE_RAFT_RAFT_POOL_H_
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "abeille.pb.h"
#include "core.hpp"
#include "peer.hpp"
#include "raft_consensus.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class RaftConsensus;
class Peer;

class RaftPool {
public:
  // Init all peers with raft_consensus
  explicit RaftPool(std::shared_ptr<RaftConsensus> raft);
  ~RaftPool();

  // Launches peer_thread_main from raft_consensus for all peers
  // FIXME: impelement
  void Run();

  // FIXME: impelement
  void Shutdown();

  void BeginRequestVote();
  bool QuorumAll();

  // Appends Entry to all peers
  void AppendAll(const Entry &entry);

private:
  std::unordered_map<uint64_t, Peer*> peers_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_RAFT_POOL_H_
