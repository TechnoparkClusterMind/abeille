#include "user/include/data_processor.hpp"

namespace abeille {
namespace user {

error ProcessData(const TaskData &task_data, TaskResult *task_result) {
  int sum = 0;
  for (int elem : task_data.data()) {
    sum += elem;
  }

  task_result->set_result(sum);

  return error();
}

}  // namespace user
}  // namespace abeille
