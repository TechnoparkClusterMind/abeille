add_library(utils src/convert.cpp src/logger.cpp src/cli.cpp)
target_include_directories(utils PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(utils PUBLIC rpc)
