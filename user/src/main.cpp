#include <vector>

#include "client.hpp"
#include "constants.hpp"

int main() {
  User::Client user_client(USER_CLIENT_HOST);

  std::vector<std::vector<int>> data = {{1, 2}, {3, 4}};
  user_client.StreamData(data);

  return 0;
}
