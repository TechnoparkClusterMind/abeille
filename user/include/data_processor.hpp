#ifndef ABEILLE_USER_DATA_PROCESSOR_HPP_
#define ABEILLE_USER_DATA_PROCESSOR_HPP_

#include "abeille.grpc.pb.h"
#include "errors.hpp"

using grpc::Channel;
using grpc::ClientContext;

namespace abeille {
namespace user {

error ProcessData(const TaskData &task_data, TaskResult *task_result);

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_DATA_PROCESSOR_HPP_
