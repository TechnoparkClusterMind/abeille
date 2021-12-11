#include "user/include/commands.hpp"

#include <google/protobuf/util/json_util.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "user/include/registry.hpp"
#include "utils/include/errors.hpp"
#include "utils/include/logger.hpp"

namespace abeille {
namespace user {
namespace commands {

using google::protobuf::util::JsonStringToMessage;

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
    return "invalid usage";
  }

  // try to open the provided file
  std::string filepath = args[1];
  std::ifstream file(filepath);
  if (!file.good()) {
    return "failed to open the file";
  }

  // get the string representation of the file
  std::stringstream input;
  input << file.rdbuf();

  // and convert it into the proto message
  auto task_data = new TaskData();
  auto status = JsonStringToMessage(input.str(), task_data);
  if (!status.ok()) {
    return status.message().as_string();
  }

  error err = Wrapper::user_client.UploadData(task_data);
  if (!err.ok()) {
    return err.what();
  }

  // add the filepath to the registry for future identification
  Registry::Instance().filepaths.push_back(filepath);

  return "successfully uploaded the data";
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
