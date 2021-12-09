#include "user/include/commands.hpp"

#include <exception>
#include <fstream>

#include "logger.hpp"

namespace abeille {
namespace user {
namespace commands {

Wrapper::Wrapper() noexcept {
  handlers = {{"ud", UploadData}, {"status", Status}};
  helper = {{"ud <filepath>",
             "uploads data from the <filepath> to abeille (json format with "
             "accordance to the proto file)"},
            {"status", "prints current status of all tasks"}};
}

void Wrapper::RunUserClient() { user_client.Run(); }

std::string UploadData(CLI::args_type args) {
  if (args.size() != 2) {
    return "error: invalid usage";
  }

  std::string filepath = args[1];
  std::ifstream file(filepath);
  if (!file.good()) {
    LOG_TRACE();
    return "failed to open the file";
  }

  std::stringstream input;
  input << file.rdbuf();

  auto task = new TaskData();
  auto status = google::protobuf::util::JsonStringToMessage(input.str(), task);
  if (!status.ok()) {
    return status.message().ToString();
  }

  // auto response = Wrapper::user_client.UploadData(task);
  Wrapper::user_client.UploadData(task);

  std::stringstream output;
  // output << "task id is [" << response.task_id() << "]";

  return output.str();
}

std::string Status(CLI::args_type args) {
  if (args.size() < 2) {
    return "error: invalid usage";
  }
  // TODO: implement me
  return "status";
}

}  // namespace commands
}  // namespace user
}  // namespace abeille
