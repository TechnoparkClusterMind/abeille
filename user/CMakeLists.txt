add_executable(user_client cmd/main.cpp src/user_client.cpp)
target_include_directories(user_client PUBLIC include)
target_link_libraries(user_client PUBLIC rpc_global utils)

