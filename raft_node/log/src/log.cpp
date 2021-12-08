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

void Log::Append(const Entry& entry) noexcept { log_.push_back(entry); }

Term Log::LastTerm() const noexcept {
  if (log_.empty()) {
    return 0;
  }
  return log_.back().term();
}

Index Log::LastIndex() const noexcept {
    return log_.size();
}

bool Log::Exists(Index index) const noexcept { return index <= log_.size(); }

bool Log::Check(Index index, Term term) const noexcept {
  if (index > log_.size() || log_[index - 1].term() != term) {
    return false;
  }
  return true;
}

Entry* Log::GetEntry(Index index) noexcept {
  assert(index != 0);
  return index > log_.size() || log_.size() == 0 ? nullptr : &log_[index - 1];
}

}  // namespace raft_node
}  // namespace abeille
