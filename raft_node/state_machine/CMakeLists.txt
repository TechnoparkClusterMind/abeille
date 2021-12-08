add_library(state_machine src/state_machine.cpp)
target_include_directories(state_machine PUBLIC include)
target_link_libraries(state_machine PUBLIC utils thread_pools log)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
