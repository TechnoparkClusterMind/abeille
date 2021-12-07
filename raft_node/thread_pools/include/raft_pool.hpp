#ifndef ABEILLE_RAFT_RAFT_POOL_H_
#define ABEILLE_RAFT_RAFT_POOL_H_
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

#include "abeille.pb.h"
#include "core.hpp"
#include "peer.hpp"
#include "local_node.hpp"
#include "raft_consensus.hpp"

namespace abeille {
namespace raft_node {

// forward declaration
class RaftConsensus;
class Peer;

class RaftPool {
public:
  typedef std::shared_ptr<Peer> PeerRef;
  typedef std::shared_ptr<RaftConsensus> RaftRef;

  // Init all peers with raft_consensus
  explicit RaftPool(RaftRef raft);
  ~RaftPool() = default;

  // Launches peer_thread_main from raft_consensus for all peers
  void Run();
  void Shutdown();

  // FIXME: impelement !!!:
  void BeginRequestVote(){};
  bool MajorityVotes() { return true; };
  // Appends Entry to all peers
  void AppendAll(const Entry &entry){};

private:
  std::unordered_map<uint64_t, PeerRef> peers_;
  RaftRef raft_;
};

} // namespace raft_node
} // namespace abeille

#endif // ABEILLE_RAFT_RAFT_POOL_H_
