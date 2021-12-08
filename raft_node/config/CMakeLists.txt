add_library(config src/config.cpp)
target_include_directories(config PUBLIC include)
target_link_libraries(config PUBLIC utils rpc_global)
