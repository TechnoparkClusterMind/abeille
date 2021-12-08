add_executable(worker_client cmd/main.cpp src/worker_client.cpp)
target_include_directories(worker_client PUBLIC include)
target_link_libraries(worker_client PUBLIC rpc_global data_processor utils)
