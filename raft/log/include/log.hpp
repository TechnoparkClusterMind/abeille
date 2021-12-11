#ifndef ABEILLE_RAFT_LOG_H_
#define ABEILLE_RAFT_LOG_H_

#include <string>
#include <vector>

#include "rpc/proto/abeille.grpc.pb.h"
#include "utils/include/types.hpp"

namespace abeille {
namespace raft {

// FIXME: refactor to sigleton

// Indexing starts with 1 !!!
class Log {
 public:
  typedef std::vector<Entry>::iterator iterator;
  typedef std::vector<Entry>::const_iterator const_iterator;
  typedef const Entry& EntryConstReference;
  typedef Entry& EntryReference;
  typedef Entry* EntryPointer;

  Log() noexcept = default;
  // explicit Log(const std::string& storage_dir);
  ~Log() noexcept = default;

  // Purge removes all entries with index greater than the provided one and
  // returns their amount
  size_t Purge(Index index) noexcept;

  // Append appends an entry to log
  void Append(EntryConstReference entry) noexcept;

  // LastTerm returns last registered term/index in the log
  Term LastTerm() const noexcept;
  Index LastIndex() const noexcept;

  EntryPointer GetEntry(Index index) noexcept;

  iterator begin() noexcept;
  const_iterator cbegin() const noexcept;
  iterator end() noexcept;
  const_iterator cend() const noexcept;

  EntryReference operator[](Index pos);
  EntryConstReference operator[](Index pos) const;

 private:
  std::vector<Entry> log_;
};

}  // namespace raft
}  // namespace abeille

#endif  // ABEILLE_RAFT_LOG_H_
