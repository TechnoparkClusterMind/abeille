find_package(Protobuf REQUIRED)
add_executable(user_client cmd/main.cpp src/user_client.cpp src/commands.cpp)
target_include_directories(user_client PUBLIC include)
target_link_libraries(user_client PUBLIC rpc_global utils linenoise ${Protobuf_LIBRARIES})

add_library(data_processor src/data_processor.cpp)
target_include_directories(data_processor PUBLIC include)
target_link_libraries(data_processor PUBLIC rpc_global utils)

