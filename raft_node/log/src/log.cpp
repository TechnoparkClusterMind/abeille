#include "log.hpp"

namespace abeille {
namespace raft_node {

size_t Log::Purge(Index index) noexcept {
  size_t count = 0;
  while (log_.size() > index) {
    ++count;
    log_.pop_back();
  }
  return count;
}

void Log::Append(const Entry &entry) noexcept { log_.push_back(entry); }

Term Log::LastTerm() const noexcept {
  if (log_.empty()) {
    return 0;
  }
  return log_.front().term();
}

Index Log::LastIndex() const noexcept { return log_.size() - 1; }

bool Log::Exists(Index index) const noexcept { return index < log_.size(); }

bool Log::Check(Index index, Term term) const noexcept {
  if (index >= log_.size() || log_[index].term() != term) {
    return false;
  }
  return true;
}

Entry Log::GetEntry(Index index) { return log_[index]; }

} // namespace raft_node
} // namespace abeille
