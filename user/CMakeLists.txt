find_package(Protobuf REQUIRED)

add_executable(user_client 
  cmd/main.cpp
  src/user_client.cpp
  src/commands.cpp
)
target_include_directories(user_client PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(user_client PUBLIC rpc utils linenoise ${Protobuf_LIBRARIES} common)

add_custom_command(TARGET user_client POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/user/data $<TARGET_FILE_DIR:user_client>/data)

add_library(data_processor src/data_processor.cpp)
target_include_directories(data_processor PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(data_processor PUBLIC rpc utils)

