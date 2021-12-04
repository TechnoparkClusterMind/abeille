find_package(gRPC REQUIRED)
add_executable(worker_service cmd/main.cpp src/worker_service.cpp)
target_include_directories(worker_service PUBLIC include)
target_link_libraries(worker_service PUBLIC utils)
