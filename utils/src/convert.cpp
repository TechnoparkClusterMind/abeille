#include "convert.hpp"

uint64_t address2uint(const std::string &address) {
  size_t pos = address.find(':');
  uint64_t port = std::stoull(address.substr(pos + 1));

  size_t index = 0;
  size_t offset = 0;
  std::string ip = address.substr(0, pos);

  uint64_t splitted = 0;
  while ((pos = ip.find('.', offset)) != std::string::npos) {
    uint64_t piece = std::stoull(ip.substr(offset, pos));
    splitted |= piece << shift_mask[index++];
    offset = pos + 1;
  }

  uint64_t piece = std::stoull(ip.substr(offset));
  splitted |= piece << shift_mask[index];

  splitted |= port & 0xFFFF;

  return splitted;
}

std::string uint2address(uint64_t n) {
  return std::to_string(((n >> 40) & 0xFF)) + "." +
         std::to_string(((n >> 32) & 0xFF)) + "." +
         std::to_string(((n >> 24) & 0xFF)) + "." +
         std::to_string(((n >> 16) & 0xFF)) + ":" + std::to_string(n & 0xFFFF);
}

std::string ExtractAddress(const std::string &str) {
  return str.substr(str.find(':') + 1);
}

std::vector<std::string> Split(const std::string &str, char ch) {
  // here we do not ever increase the value of 'left' because of possible
  // overflow

  auto left = str.find_first_not_of(ch);
  auto right = str.find_first_of(ch, left);

  std::vector<std::string> splitted;
  while (right != std::string::npos) {
    splitted.push_back(str.substr(left, right - left));
    left = str.find_first_not_of(ch, right + 1);
    right = str.find_first_of(ch, left);
  }

  // we arrive either at the last word or at a trailing delimiter
  if (left != std::string::npos) {
    splitted.push_back(str.substr(left, str.size()));
  }

  return splitted;
}
