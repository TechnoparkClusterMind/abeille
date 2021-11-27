find_package(gRPC REQUIRED)
add_library(rpc
            protos/abeille.pb.cc protos/abeille.grpc.pb.cc
            src/raft_server.cpp src/raft_service.cpp
            src/user_server.cpp src/user_service.cpp)
target_include_directories(rpc PUBLIC include protos)
target_link_libraries(rpc PUBLIC gRPC::grpc++)
