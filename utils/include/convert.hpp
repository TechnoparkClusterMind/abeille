#ifndef ABEILLE_UTILS_CONVERT_H_
#define ABEILLE_UTILS_CONVERT_H_

#include "abeille.grpc.pb.h"

TaskData* RawData2TaskData(const std::vector<int>& raw_data) {
  TaskData* task_data = new TaskData();
  for (int value : raw_data) {
    task_data->add_data(value);
  }
  return task_data;
}

// TODO: add host converter

#endif  // ABEILLE_UTILS_CONVERT_H_
