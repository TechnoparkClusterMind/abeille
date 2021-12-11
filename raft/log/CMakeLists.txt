add_library(log src/log.cpp)
target_include_directories(log PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(log PUBLIC utils rpc)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
