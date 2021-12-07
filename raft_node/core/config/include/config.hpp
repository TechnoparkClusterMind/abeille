#ifndef ABEILLE_CONFIG_H
#define ABEILLE_CONFIG_H

#include <string>
#include <vector>
#include <fstream>

#include "types.hpp"

namespace abeille {

class Config {
 public:
  typedef std::vector<std::string> Container;

  Config() = default;
  Config(Config &&conf) = default;
  Config &operator=(const Config &) = default;
  Config &operator=(Confgi &&) = default;
  ~Config() = default;

  void SetId(ServerId id) noexcept { id_ = id; }
  void SetPeers(Container &&peers) noexcept;
  void SetWorkers(Container &&workers) noexcept;
  void SetUserAddress(std::string &&address) noexcept;
  void SetRaftAddress(std::string &&address) noexcept;
  void SetWorkerAddress(std::string &&address) noexcept;

  void parse(std::ifstream& file);
  // FIXME: will be deleted
  void init() noexcept;

  uint64_t GetId() const noexcept { return id_; }
  std::string GetUserAddress() const noexcept { return user_address_; }
  std::string GetRaftAddress() const noexcept { return raft_address_; }
  std::string GetWorkerAddress() const noexcept { return worker_address_; }
  const Container &GetPeers() const noexcept { return peers_; }
  const Container &GetWorkers() const noexcept { return workers_; }

 private:
  std::vector<std::string> peers_;
  std::vector<std::string> workers_;
  std::string user_address_;
  std::string raft_address_;
  std::string worker_address_;
  uint64_t id_;
};

}  // namespace abeille

#endif  // ABEILLE_CONFIG_H
