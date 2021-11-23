#ifndef ABEILLE_RAFT_PEER_H_
#define ABEILLE_RAFT_PEER_H_

#include <grpcpp/channel.h>

#include <memory>
#include <queue>
#include <thread>

#include "raft_entry.pb.h"
#include "raft_service.pb.h"

namespace Abeille {
namespace Raft_node {

// Tracks various bits of state for each server, wich is used
// when we are the candidate or the leader
class Peer {
   public:
    // grpc stub is initialized with channel
    explicit Peer(std::shared_ptr<grpc::Channel> channel,
                  std::shared_ptr<Raft_consensus> raft, uint64_t id);

    // destructor
    ~Peer();

    // Run peer_thread_main from Raft_consensus
    void run();

    // id of the current raft_node
    uint64_t id;

    // to this queue entries are pushed (from task_manager)
    std::queue<Proto::Entry> entries;
    std::thread peer_thread;

   private:
    // for RPCs
    std::unique_ptr<Proto::Raft_service> stub;
    std::shared_ptr<Raft_consensus> raft;

    // for peer_thread_main to know when shutdown
    bool is_exiting;

    // TODO: ...
};

}  // namespace Raft_node
}  // namespace Abeille

#endif  // ABEILLE_RAFT_PEER_H_
