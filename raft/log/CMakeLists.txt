add_library(log src/log.cpp)
target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC common)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
