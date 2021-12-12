#ifndef ABEILLE_USER_DATA_PROCESSOR_HPP_
#define ABEILLE_USER_DATA_PROCESSOR_HPP_

#include "user/model/proto/task.pb.h"
#include "utils/include/errors.hpp"

namespace abeille {
namespace user {

error ProcessData(const Task::Data &task_data, Task::Result &task_result);

}  // namespace user
}  // namespace abeille

#endif  // ABEILLE_USER_DATA_PROCESSOR_HPP_
