#ifndef ABEILLE_RAFT_LOG_H
#define ABEILLE_RAFT_LOG_H

#include <string>
#include <vector>

#include "abeille.grpc.pb.h"
#include "common.hpp"

class Log {
 public:
  Log() noexcept = default;
  // explicit Log(const std::string& storage_dir);
  ~Log() noexcept = default;

  // Purge removes all entries with index greater than the provided one and returns their amount
  size_t Purge(Index index) noexcept;

  // Append appends an entry to log
  void Append(const Entry& entry) noexcept;

  // LastTerm returns last registered term in the log
  Term LastTerm() const noexcept;

  // LastIndex returns last registered index in the log
  Index LastIndex() const noexcept;

  // Exists checks if given index is present in the log
  bool Exists(Index index) const noexcept;

  // Check checks if an entry with given term and index is present in the log
  bool Check(Index index, Term term) const noexcept;

 private:
  std::vector<Entry> log_;
};

#endif  // ABEILLE_RAFT_LOG_H
