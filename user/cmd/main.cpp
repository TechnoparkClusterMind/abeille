#include <vector>

#include "client.hpp"
#include "constants.hpp"

using namespace abeille;

int main() {
  user::Client user_client(USER_CLIENT_HOST);

  std::vector<std::vector<int>> arrays = {{1, 2}, {3, 4}};
  for (const auto &data : arrays) {
    user_client.Upload(data);
  }

  return 0;
}
