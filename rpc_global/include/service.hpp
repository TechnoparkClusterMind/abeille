#ifndef ABEILLE_RPC_SERVICE_HPP_
#define ABEILLE_RPC_SERVICE_HPP_

#include <grpc/grpc.h>

#include <unordered_map>

#include "abeille.grpc.pb.h"
#include "convert.hpp"
#include "logger.hpp"
#include "raft_consensus.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace rpc {

template <typename ConnReq, typename ConnResp, typename Svc>
class Service : public Svc {
 public:
  using ConnectStream = grpc::ServerReaderWriter<ConnResp, ConnReq>;

  virtual void CommandHandler(uint64_t client_id, ConnResp *resp) = 0;
  virtual void StatusHandler(uint64_t client_id, const ConnReq *req) = 0;

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override {
    std::string address = ExtractAddress(context->peer());
    uint64_t client_id = address2uint(address);
    LOG_INFO("connection request from [%s]", address.c_str());

    auto req = std::make_unique<ConnReq>();
    while (stream->Read(req.get())) {
      auto resp = std::make_unique<ConnResp>();
      CommandHandler(client_id, resp.get());

      StatusHandler(client_id, req.get());

      bool ok = stream->Write(*resp);
      if (!ok) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    LOG_WARN("connection with [%s] was lost", address.c_str());

    return Status::OK;
  }
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_SERVICE_HPP_
