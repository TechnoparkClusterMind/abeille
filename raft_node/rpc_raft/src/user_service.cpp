
#include "user_service.hpp"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <vector>

#include "abeille.grpc.pb.h"
#include "logger.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace raft_node {

Status UserServiceImpl::Ping(ServerContext *context, const Empty *request, Empty *response) { return Status::OK; }

Status UserServiceImpl::UploadData(ServerContext *context, const UploadDataRequest *request,
                                   UploadDataResponse *response) {
  LOG_TRACE();
  Task task;
  // TODO: can we avoid this allocation?
  TaskData *task_data = new TaskData(request->task_data());
  task.set_allocated_task_data(task_data);
  task.set_id(last_task_id_);

  // FIXME: implement, send task to RaftPool
  // ...

  if (IsLeader()) {
    response->set_success(true);
    response->set_task_id(task.id());
    ++last_task_id_;
  } else {
    response->set_success(false);
    response->set_leader_id(leader_id_);
  }

  return Status::OK;
}

Status UserServiceImpl::GetResult(ServerContext *context, const GetResultRequest *request,
                                  GetResultResponse *response) {
  LOG_DEBUG("request for [%u]", request->task_id());
  TaskResult *task_result = new TaskResult();
  task_result->set_result(7);
  response->set_allocated_task_result(task_result);
  return Status::OK;
}

}  // namespace raft_node
}  // namespace abeille
