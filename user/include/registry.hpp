#ifndef ABEILLE_USER_REGISTRY_HPP_
#define ABEILLE_USER_REGISTRY_HPP_

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "rpc/proto/abeille.grpc.pb.h"

namespace abeille {
namespace user {

// TODO: needs to be refactored
struct Registry {
  static Registry &Instance() {
    static Registry instance;
    return instance;
  }

  std::queue<std::string> filenames;
  std::queue<Task::Data> task_datas;
  std::vector<Task::Result> task_results;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_REGISTRY_HPP_
