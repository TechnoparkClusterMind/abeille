#include "user/include/data_processor.hpp"

namespace abeille {
namespace user {

error ProcessData(const Task::Data &task_data, Task::Result &task_result) {
  int sum = 0;
  for (int elem : task_data.data()) {
    sum += elem;
  }

  task_result.set_result(sum);

  return error();
}

}  // namespace user
}  // namespace abeille
