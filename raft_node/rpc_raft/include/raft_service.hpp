#ifndef ABEILLE_RPC_RAFT_SERVICE_H_
#define ABEILLE_RPC_RAFT_SERVICE_H_

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"
#include "raft_consensus.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

// forward declaration
class RaftConsensus;

class RaftServiceImpl final : public RaftService::Service {
  std::shared_ptr<RaftConsensus> raft_;

 public:
  explicit RaftServiceImpl(std::shared_ptr<RaftConsensus> raft) : raft_(raft){};

  Status AppendEntry(ServerContext *context, const AppendEntryRequest *request,
                     AppendEntryResponse *response) override;
  Status RequestVote(ServerContext *context, const RequestVoteRequest *request,
                     RequestVoteResponse *response) override;
};

}  // namespace raft_node
}  // namespace abeille

#endif  // ABEILLE_RPC_RAFT_SERVICE_H_
