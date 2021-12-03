add_library(utils src/convert.cpp src/logger.cpp)
target_include_directories(utils PUBLIC include)
target_link_libraries(utils PUBLIC rpc_global)
