#include "raft/log/include/log.hpp"

#include "utils/include/logger.hpp"

namespace abeille {
namespace raft {

size_t Log::Purge(Index index) noexcept {
  size_t count = 0;
  while (log_.size() > index) {
    ++count;
    log_.pop_back();
  }
  return count;
}

void Log::Append(Log::EntryConstReference entry) noexcept {
  LOG_TRACE();
  log_.push_back(entry);
}

Term Log::LastTerm() const noexcept {
  if (log_.empty()) {
    return 0;
  }
  return log_.back().term();
}

Index Log::LastIndex() const noexcept { return log_.size(); }

Log::iterator Log::begin() noexcept { return log_.begin(); }
Log::const_iterator Log::cbegin() const noexcept { return log_.cbegin(); }
Log::iterator Log::end() noexcept { return log_.end(); }
Log::const_iterator Log::cend() const noexcept { return log_.cend(); }

Log::EntryReference Log::operator[](size_t pos) {
  assert(pos != 0);
  return log_[pos - 1];
}

Log::EntryConstReference Log::operator[](size_t pos) const {
  assert(pos != 0);
  return log_[pos - 1];
}

Log::EntryPointer Log::GetEntry(Index index) noexcept {
  assert(index != 0);
  return index > log_.size() || log_.size() == 0 ? nullptr : &log_[index - 1];
}

}  // namespace raft
}  // namespace abeille
