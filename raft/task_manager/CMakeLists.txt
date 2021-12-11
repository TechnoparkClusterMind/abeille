add_library(task_manager src/task_manager.cpp)
target_include_directories(task_manager PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(task_manager PUBLIC utils rpc core thread_pools)
