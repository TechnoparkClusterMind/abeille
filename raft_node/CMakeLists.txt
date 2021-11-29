# Raft_node compiling  (Core of this node is here too)

add_subdirectory(log)
add_subdirectory(state_machine)
add_subdirectory(thread_pools)
add_subdirectory(raft_consensus)
add_subdirectory(task_manager)
add_subdirectory(rpc_services)

add_executable(abeille-raft src/main.cpp src/core.cpp)
target_include_directories(abeille-raft PUBLIC include)
target_link_libraries(abeille-raft 
  rpc utils raft_consensus task_manager rpc_services)
