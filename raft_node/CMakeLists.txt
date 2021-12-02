# Raft_node compiling  (Core of this node is here too)

add_subdirectory(core)
add_subdirectory(rpc_raft)
add_subdirectory(log)
add_subdirectory(thread_pools)
add_subdirectory(raft_consensus)
add_subdirectory(task_manager)

add_executable(abeille-raft cmd/main.cpp)
target_link_libraries(abeille-raft core config)
