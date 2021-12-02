find_package(gRPC REQUIRED)
add_library(thread_pools src/peer.cpp src/raft_pool.cpp)
target_include_directories(thread_pools PUBLIC include)
target_link_libraries(thread_pools PUBLIC 
    raft_consensus rpc_global task_manager)
