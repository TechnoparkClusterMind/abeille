add_subdirectory(state_machine)

find_package(gRPC REQUIRED)
add_library(raft_consensus src/raft_consensus.cpp)
target_include_directories(raft_consensus PUBLIC include)
target_link_libraries(raft_consensus PUBLIC rpc_global core utils config log thread_pools)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
