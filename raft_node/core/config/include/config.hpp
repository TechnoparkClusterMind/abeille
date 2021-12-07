#ifndef ABEILLE_CONFIG_H
#define ABEILLE_CONFIG_H
#include <string>
#include <vector>

namespace abeille {

class Config {
  std::vector<std::string> peers_;
  std::vector<std::string> workers_;
  std::string user_address_;
  std::string raft_address_;
  std::string worker_address_;
  uint64_t id_;

 public:
  Config() = default;
  Config(Config &&conf) = default;
  void SetId(uint64_t id) noexcept { id_ = id; }
  void SetPeers(std::vector<std::string> &&peers) noexcept;
  void SetWorkers(std::vector<std::string> &&workers) noexcept;
  void SetUserAddress(std::string &&address) noexcept;
  void SetRaftAddress(std::string &&address) noexcept;

  // FIXME: will be deleted
  void init() noexcept;

  uint64_t GetId() const noexcept { return id_; }
  std::string GetUserAddress() const noexcept { return user_address_; }
  std::string GetRaftAddress() const noexcept { return raft_address_; }
  std::string GetWorkerAddress() const noexcept { return worker_address_; }

  ~Config() = default;
};

}  // namespace abeille

#endif  // ABEILLE_CONFIG_H
