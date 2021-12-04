find_package(gRPC REQUIRED)
add_library(rpc_raft src/user_service.cpp src/raft_service.cpp src/worker_service.cpp)

target_include_directories(rpc_raft PUBLIC include)
target_link_libraries(rpc_raft PUBLIC raft_consensus task_manager utils)
