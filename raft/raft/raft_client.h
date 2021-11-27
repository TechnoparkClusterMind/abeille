#ifndef ABEILLE_RAFT_WORKER_CLIENT_H_
#define ABEILLE_RAFT_WORKER_CLIENT_H_
#include <cstdint>
#include <unordered_map>

#include "peer.h"
#include "raft_consensus.h"
#include "raft_entry.pb.h"

namespace Abeille {
namespace Raft_node {

class Raft_client {
   public:
    // Init all peers with raft_consensus
    explicit Raft_client(shared_ptr<Raft_consensus> raft);
    ~Raft_client();

    // Launches peer_thread_main from raft_consensus for all peers
    void run();

    // Appends Entry to all peers
    void append_all(const Proto::Entry &entry);

   private:
    std::unordered_map<uint64_t, Peer> peers;
};

}  // namespace Raft_node
}  // namespace Abeille

#endif  // ABEILLE_RAFT_WORKER_CLIENT_H_
