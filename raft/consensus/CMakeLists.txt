find_package(gRPC REQUIRED)
add_library(raft_consensus src/raft_consensus.cpp)
target_include_directories(raft_consensus PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(raft_consensus PUBLIC 
  rpc core utils config log thread_pools state_machine)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
