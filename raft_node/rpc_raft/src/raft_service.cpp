#include "raft_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "abeille.grpc.pb.h"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

Status RaftServiceImpl::AppendEntry(ServerContext *context, const AppendEntryRequest *request,
                                    AppendEntryResponse *response) {
  // FIXME: implement me
  return Status::OK;
}

Status RaftServiceImpl::RequestVote(ServerContext *context, const RequestVoteRequest *request,
                                    RequestVoteResponse *response) {
  // FIXME: implement me
  return Status::OK;
}

}  // namespace raft_node
}  // namespace abeille
