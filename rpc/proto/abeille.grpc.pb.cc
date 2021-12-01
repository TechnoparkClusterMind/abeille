// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: abeille.proto

#include "abeille.pb.h"
#include "abeille.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>

static const char* UserService_method_names[] = {
  "/UserService/Ping",
  "/UserService/Upload",
  "/UserService/GetResult",
};

std::unique_ptr< UserService::Stub> UserService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< UserService::Stub> stub(new UserService::Stub(channel, options));
  return stub;
}

UserService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Ping_(UserService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Upload_(UserService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetResult_(UserService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status UserService::Stub::Ping(::grpc::ClientContext* context, const ::Empty& request, ::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall< ::Empty, ::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Ping_, context, request, response);
}

void UserService::Stub::async::Ping(::grpc::ClientContext* context, const ::Empty* request, ::Empty* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::Empty, ::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, std::move(f));
}

void UserService::Stub::async::Ping(::grpc::ClientContext* context, const ::Empty* request, ::Empty* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::Empty>* UserService::Stub::PrepareAsyncPingRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::Empty, ::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Ping_, context, request);
}

::grpc::ClientAsyncResponseReader< ::Empty>* UserService::Stub::AsyncPingRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncPingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status UserService::Stub::Upload(::grpc::ClientContext* context, const ::UploadRequest& request, ::UploadResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::UploadRequest, ::UploadResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Upload_, context, request, response);
}

void UserService::Stub::async::Upload(::grpc::ClientContext* context, const ::UploadRequest* request, ::UploadResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::UploadRequest, ::UploadResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Upload_, context, request, response, std::move(f));
}

void UserService::Stub::async::Upload(::grpc::ClientContext* context, const ::UploadRequest* request, ::UploadResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Upload_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::UploadResponse>* UserService::Stub::PrepareAsyncUploadRaw(::grpc::ClientContext* context, const ::UploadRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::UploadResponse, ::UploadRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Upload_, context, request);
}

::grpc::ClientAsyncResponseReader< ::UploadResponse>* UserService::Stub::AsyncUploadRaw(::grpc::ClientContext* context, const ::UploadRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncUploadRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status UserService::Stub::GetResult(::grpc::ClientContext* context, const ::GetResultRequest& request, ::GetResultResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::GetResultRequest, ::GetResultResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetResult_, context, request, response);
}

void UserService::Stub::async::GetResult(::grpc::ClientContext* context, const ::GetResultRequest* request, ::GetResultResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::GetResultRequest, ::GetResultResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetResult_, context, request, response, std::move(f));
}

void UserService::Stub::async::GetResult(::grpc::ClientContext* context, const ::GetResultRequest* request, ::GetResultResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetResult_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::GetResultResponse>* UserService::Stub::PrepareAsyncGetResultRaw(::grpc::ClientContext* context, const ::GetResultRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::GetResultResponse, ::GetResultRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetResult_, context, request);
}

::grpc::ClientAsyncResponseReader< ::GetResultResponse>* UserService::Stub::AsyncGetResultRaw(::grpc::ClientContext* context, const ::GetResultRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetResultRaw(context, request, cq);
  result->StartCall();
  return result;
}

UserService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      UserService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< UserService::Service, ::Empty, ::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](UserService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::Empty* req,
             ::Empty* resp) {
               return service->Ping(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      UserService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< UserService::Service, ::UploadRequest, ::UploadResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](UserService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::UploadRequest* req,
             ::UploadResponse* resp) {
               return service->Upload(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      UserService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< UserService::Service, ::GetResultRequest, ::GetResultResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](UserService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::GetResultRequest* req,
             ::GetResultResponse* resp) {
               return service->GetResult(ctx, req, resp);
             }, this)));
}

UserService::Service::~Service() {
}

::grpc::Status UserService::Service::Ping(::grpc::ServerContext* context, const ::Empty* request, ::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status UserService::Service::Upload(::grpc::ServerContext* context, const ::UploadRequest* request, ::UploadResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status UserService::Service::GetResult(::grpc::ServerContext* context, const ::GetResultRequest* request, ::GetResultResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* RaftService_method_names[] = {
  "/RaftService/AppendEntry",
  "/RaftService/RequestVote",
};

std::unique_ptr< RaftService::Stub> RaftService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< RaftService::Stub> stub(new RaftService::Stub(channel, options));
  return stub;
}

RaftService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_AppendEntry_(RaftService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_RequestVote_(RaftService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status RaftService::Stub::AppendEntry(::grpc::ClientContext* context, const ::AppendEntryRequest& request, ::AppendEntryResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::AppendEntryRequest, ::AppendEntryResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_AppendEntry_, context, request, response);
}

void RaftService::Stub::async::AppendEntry(::grpc::ClientContext* context, const ::AppendEntryRequest* request, ::AppendEntryResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::AppendEntryRequest, ::AppendEntryResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_AppendEntry_, context, request, response, std::move(f));
}

void RaftService::Stub::async::AppendEntry(::grpc::ClientContext* context, const ::AppendEntryRequest* request, ::AppendEntryResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_AppendEntry_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::AppendEntryResponse>* RaftService::Stub::PrepareAsyncAppendEntryRaw(::grpc::ClientContext* context, const ::AppendEntryRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::AppendEntryResponse, ::AppendEntryRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_AppendEntry_, context, request);
}

::grpc::ClientAsyncResponseReader< ::AppendEntryResponse>* RaftService::Stub::AsyncAppendEntryRaw(::grpc::ClientContext* context, const ::AppendEntryRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncAppendEntryRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status RaftService::Stub::RequestVote(::grpc::ClientContext* context, const ::RequestVoteRequest& request, ::RequestVoteResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::RequestVoteRequest, ::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_RequestVote_, context, request, response);
}

void RaftService::Stub::async::RequestVote(::grpc::ClientContext* context, const ::RequestVoteRequest* request, ::RequestVoteResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::RequestVoteRequest, ::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RequestVote_, context, request, response, std::move(f));
}

void RaftService::Stub::async::RequestVote(::grpc::ClientContext* context, const ::RequestVoteRequest* request, ::RequestVoteResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RequestVote_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::RequestVoteResponse>* RaftService::Stub::PrepareAsyncRequestVoteRaw(::grpc::ClientContext* context, const ::RequestVoteRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::RequestVoteResponse, ::RequestVoteRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_RequestVote_, context, request);
}

::grpc::ClientAsyncResponseReader< ::RequestVoteResponse>* RaftService::Stub::AsyncRequestVoteRaw(::grpc::ClientContext* context, const ::RequestVoteRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncRequestVoteRaw(context, request, cq);
  result->StartCall();
  return result;
}

RaftService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RaftService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RaftService::Service, ::AppendEntryRequest, ::AppendEntryResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](RaftService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::AppendEntryRequest* req,
             ::AppendEntryResponse* resp) {
               return service->AppendEntry(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RaftService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RaftService::Service, ::RequestVoteRequest, ::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](RaftService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::RequestVoteRequest* req,
             ::RequestVoteResponse* resp) {
               return service->RequestVote(ctx, req, resp);
             }, this)));
}

RaftService::Service::~Service() {
}

::grpc::Status RaftService::Service::AppendEntry(::grpc::ServerContext* context, const ::AppendEntryRequest* request, ::AppendEntryResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status RaftService::Service::RequestVote(::grpc::ServerContext* context, const ::RequestVoteRequest* request, ::RequestVoteResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


