add_library(raft_consensus src/raft_consensus.cpp)
target_include_directories(raft_consensus PUBLIC include)
target_link_libraries(raft_consensus PUBLIC 
  utils proto config log thread_pools)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
