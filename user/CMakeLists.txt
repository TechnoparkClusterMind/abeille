find_package(Protobuf REQUIRED)
add_executable(user_client cmd/main.cpp src/user_client.cpp)
target_include_directories(user_client PUBLIC include)
target_link_libraries(user_client PUBLIC rpc_global utils linenoise ${Protobuf_LIBRARIES})

add_custom_command(TARGET user_client POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/user/data $<TARGET_FILE_DIR:user_client>/data)

