add_subdirectory(rpc)
add_subdirectory(log)
add_subdirectory(core)
add_subdirectory(config)
add_subdirectory(consensus)
add_subdirectory(thread_pools)
add_subdirectory(task_manager)
add_subdirectory(state_machine)

add_executable(abeille-raft cmd/main.cpp)
target_link_libraries(abeille-raft core config)

add_custom_command(TARGET abeille-raft POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/raft/example $<TARGET_FILE_DIR:abeille-raft>/example)
