find_package(gRPC REQUIRED)
add_executable(worker_server cmd/main.cpp src/worker_service.cpp)
target_include_directories(worker_server PUBLIC include)
target_link_libraries(worker_server PUBLIC utils)
