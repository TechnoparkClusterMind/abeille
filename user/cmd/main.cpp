#include <vector>

#include "client.hpp"
#include "constants.hpp"

using namespace abeille;

int main() {
  user::Client user_client(USER_CLIENT_HOST);

  std::vector<std::vector<int>> data = {{1, 2}, {3, 4}};
  user_client.StreamData(data);

  return 0;
}
