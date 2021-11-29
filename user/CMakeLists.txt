add_executable(user_client src/client.cpp cmd/main.cpp)
target_include_directories(user_client PUBLIC include)
target_link_libraries(user_client PUBLIC rpc utils)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
