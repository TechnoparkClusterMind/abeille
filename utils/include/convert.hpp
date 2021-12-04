#ifndef ABEILLE_UTILS_CONVERT_H_
#define ABEILLE_UTILS_CONVERT_H_

#include <cstdint>
#include <string>

#include "abeille.grpc.pb.h"

static const uint64_t shift_mask[] = {40, 32, 24, 16};

uint64_t address2uint(const std::string& address);

std::string uint2address(uint64_t n);

// ExtractAddress extracts address from strings of the form "protocol:ip:port"
std::string ExtractAddress(const std::string& str);

TaskData* RawData2TaskData(const std::vector<int>& raw_data);

#endif  // ABEILLE_UTILS_CONVERT_H_
