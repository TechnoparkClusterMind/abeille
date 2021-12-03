#include <string>
#include <vector>

#include "constants.hpp"
#include "convert.hpp"
#include "logger.hpp"
#include "user_client.hpp"

using abeille::user::Client;

// TODO: implement cli for user

int main() {
  std::vector<std::string> addresses = {"127.0.0.1:50051", "127.0.0.1:50052"};
  Client user_client(addresses);

  std::vector<std::vector<int>> arrays = {{1, 2}, {3, 4}};
  for (const auto &data : arrays) {
    auto response = user_client.UploadData(RawData2TaskData(data));
    LOG_DEBUG("UploadData response: task_id = %u", response.task_id());
  }

  return 0;
}
