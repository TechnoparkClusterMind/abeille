#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"
#include "user_client.hpp"

using namespace abeille;

// TODO: implement cli for user

int main() {
  user::Client user_client(USER_SERVICE_HOST);

  std::vector<std::vector<int>> arrays = {{1, 2}, {3, 4}};
  for (const auto &data : arrays) {
    auto response = user_client.UploadData(RawData2TaskData(data));
    LOG_DEBUG("UploadData response: task_id = %u", response.task_id());
  }

  return 0;
}
