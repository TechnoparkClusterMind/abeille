#include "raft/rpc/include/raft_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "rpc/proto/abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft {

Status RaftServiceImpl::AppendEntry(ServerContext *context,
                                    const AppendEntryRequest *request,
                                    AppendEntryResponse *response) {
  raft_->HandleAppendEntry(request, response);
  return Status::OK;
}

Status RaftServiceImpl::RequestVote(ServerContext *context,
                                    const RequestVoteRequest *request,
                                    RequestVoteResponse *response) {
  raft_->HandleRequestVote(request, response);
  return Status::OK;
}

}  // namespace raft
}  // namespace abeille
