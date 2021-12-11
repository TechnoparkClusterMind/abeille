add_library(core src/core.cpp)
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(core PUBLIC rpc config thread_pools rpc_raft utils raft_consensus task_manager)
