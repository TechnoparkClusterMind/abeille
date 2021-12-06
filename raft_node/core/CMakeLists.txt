add_subdirectory(config)
add_library(core src/core.cpp)
target_include_directories(core PUBLIC include)
target_link_libraries(core PUBLIC rpc_global config rpc_raft utils raft_consensus task_manager)
