add_library(state_machine src/state_machine.cpp)
target_include_directories(state_machine PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(state_machine PUBLIC task_manager utils thread_pools core)
