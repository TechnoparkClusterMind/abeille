#ifndef ABEILLE_RPC_SERVICE_HPP_
#define ABEILLE_RPC_SERVICE_HPP_

#include <grpc/grpc.h>

#include <unordered_map>

#include "raft/consensus/include/raft_consensus.hpp"
#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/convert.hpp"
#include "utils/include/logger.hpp"

using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

namespace abeille {
namespace rpc {

template <typename ConnReq, typename ConnResp, typename Svc>
class Service : public Svc {
 public:
  using ConnectStream = grpc::ServerReaderWriter<ConnResp, ConnReq>;

  virtual void ConnectHandler(uint64_t client_id) {}
  virtual void CommandHandler(uint64_t client_id, ConnResp &resp) = 0;
  virtual void StatusHandler(uint64_t client_id, const ConnReq &req) = 0;
  virtual void DisconnectHandler(uint64_t client_id) {}

 private:
  Status Connect(ServerContext *context, ConnectStream *stream) override {
    std::string address = ExtractAddress(context->peer());
    uint64_t client_id = address2uint(address);

    ConnectHandler(client_id);
    LOG_INFO("connection request from [%s]", address.c_str());

    ConnReq req;
    while (stream->Read(&req)) {
      ConnResp resp;
      CommandHandler(client_id, resp);

      StatusHandler(client_id, req);

      bool ok = stream->Write(resp);
      if (!ok) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    DisconnectHandler(client_id);
    LOG_WARN("connection with [%s] was lost", address.c_str());

    return Status::OK;
  }
};

}  // namespace rpc
}  // namespace abeille

#endif  // ABEILLE_RPC_SERVICE_HPP_
