add_executable(worker_client cmd/main.cpp src/worker_client.cpp)
target_include_directories(worker_client PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(worker_client PUBLIC rpc data_processor utils)
