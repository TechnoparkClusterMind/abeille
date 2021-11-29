find_package(gRPC REQUIRED)
add_library(rpc_services src/raft_service.cpp src/user_service.cpp)
target_include_directories(rpc_services PUBLIC include)
target_link_libraries(rpc_services PUBLIC utils proto gRPC::grpc++)
