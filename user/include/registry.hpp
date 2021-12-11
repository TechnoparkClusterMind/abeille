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

  std::vector<std::string> filepaths;
  std::queue<TaskData *> task_datas;
  std::vector<TaskResult *> task_results;
};

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_REGISTRY_HPP_
