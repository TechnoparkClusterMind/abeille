#ifndef ABEILLE_CONFIG_H
#define ABEILLE_CONFIG_H

#include <string>
#include <vector>
#include <fstream>

#include "types.hpp"

namespace abeille {

class Config {
 public:
  typedef std::vector<std::string> AddressContainer;

  Config() = default;
  Config(Config &&conf) = default;
  Config &operator=(const Config &) = default;
  Config &operator=(Config &&) = default;
  ~Config() = default;

  void SetPeers(AddressContainer &peers) noexcept;
  void SetWorkers(AddressContainer &workers) noexcept;
  void SetUserAddress(std::string &address) noexcept;
  void SetWorkerAddress(std::string &address) noexcept;
  // This one has to init id too
  void SetRaftAddress(std::string &address) noexcept;

  bool is_ok() const noexcept;

  // FIXME: will be deleted
  void init() noexcept;
  // FIXME: not implemented
  void parse(std::ifstream& file);

  uint64_t GetId() const noexcept { return id_; }
  std::string GetUserAddress() const noexcept { return user_address_; }
  std::string GetRaftAddress() const noexcept { return raft_address_; }
  std::string GetWorkerAddress() const noexcept { return worker_address_; }
  const AddressContainer &GetPeers() const noexcept { return peers_; }
  const AddressContainer &GetWorkers() const noexcept { return workers_; }

 private:
  AddressContainer peers_;
  AddressContainer workers_;
  std::string user_address_;
  std::string raft_address_;
  std::string worker_address_;
  uint64_t id_ = 0;
};

}  // namespace abeille

#endif  // ABEILLE_CONFIG_H
