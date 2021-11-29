add_library(task_manager src/task_manager.cpp)
target_include_directories(task_manager PUBLIC include)
target_link_libraries(task_manager PUBLIC 
  utils proto gRPC::grpc++ thread_pools)
