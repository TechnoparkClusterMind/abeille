#include "convert.hpp"

TaskData* RawData2TaskData(const std::vector<int>& raw_data) {
  TaskData* task_data = new TaskData();
  for (int value : raw_data) {
    task_data->add_data(value);
  }
  return task_data;
}

uint64_t address2uint(const std::string& address) {
  size_t pos = address.find(':');
  uint64_t port = std::stoull(address.substr(pos + 1));

  size_t index = 0;
  size_t offset = 0;
  std::string ip = address.substr(0, pos);

  uint64_t result = 0;
  while ((pos = ip.find('.', offset)) != std::string::npos) {
    uint64_t piece = std::stoull(ip.substr(offset, pos));
    result |= piece << shift_mask[index++];
    offset = pos + 1;
  }

  uint64_t piece = std::stoull(ip.substr(offset));
  result |= piece << shift_mask[index];

  result |= port & 0xFFFF;

  return result;
}

std::string uint2address(uint64_t n) {
  return std::to_string(((n >> 40) & 0xFF)) + "." + std::to_string(((n >> 32) & 0xFF)) + "." +
         std::to_string(((n >> 24) & 0xFF)) + "." + std::to_string(((n >> 16) & 0xFF)) + ":" +
         std::to_string(n & 0xFFFF);
}