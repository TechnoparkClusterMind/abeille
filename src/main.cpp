#include "constants.hpp"
#include "user_server.hpp"

int main() {
  rpc::UserServer user_server(USER_SERVICE_HOST);
  user_server.Run();
  return 0;
}
