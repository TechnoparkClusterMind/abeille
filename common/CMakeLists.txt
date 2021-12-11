add_library(common src/client_config.cpp)
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(common PUBLIC utils)
