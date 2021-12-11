add_library(config src/config.cpp)
target_include_directories(config PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(config PUBLIC utils rpc)
