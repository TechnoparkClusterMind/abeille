#ifndef ABEILLE_RPC_RAFT_SERVICE_H_
#define ABEILLE_RPC_RAFT_SERVICE_H_

#include <memory>
#include <vector>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace rpc {

class RaftServiceImpl final : public RaftService::Service {
  // RaftServiceImpl() FIXME: implement me, pass RaftConsensus here
  Status AppendEntry(ServerContext *context, const AppendEntryRequest *request, AppendEntryResponse *response) override;
  Status RequestVote(ServerContext *context, const RequestVoteRequest *request, RequestVoteResponse *response) override;
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_RAFT_SERVICE_H_
