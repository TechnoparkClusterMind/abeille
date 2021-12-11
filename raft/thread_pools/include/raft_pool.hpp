#ifndef ABEILLE_RAFT_RAFT_POOL_H_
#define ABEILLE_RAFT_RAFT_POOL_H_
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

#include "raft/consensus/include/raft_consensus.hpp"
#include "raft/core/include/core.hpp"
#include "raft/thread_pools/include/peer.hpp"
#include "rpc/proto/abeille.pb.h"
#include "utils/include/errors.hpp"
#include "utils/include/types.hpp"

namespace abeille {
namespace raft {

// forward declaration
class RaftConsensus;
class Peer;

class RaftPool {
 public:
  typedef std::shared_ptr<Peer> PeerRef;
  typedef std::shared_ptr<RaftConsensus> RaftRef;
  typedef error Status;

  // Init all peers with raft_consensus
  explicit RaftPool(RaftRef raft);
  ~RaftPool() = default;

  // Launches peer_thread_main from raft_consensus for all peers
  Status Run();
  void Shutdown();
  void AppendAll(const Entry& entry);

 private:
  Index poolCommitIndex(Index& prev_commit_idx);
  void beginRequestVote();
  bool majorityVotes();

  // std::unordered_map<ServerId, PeerRef> peers_;
  std::vector<PeerRef> peers_;
  RaftRef raft_;

  friend class RaftConsensus;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_RAFT_POOL_H_
