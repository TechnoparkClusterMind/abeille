find_package(gRPC REQUIRED)
add_library(rpc
            src/server.cpp src/registry.cpp
            protos/abeille.pb.cc protos/abeille.grpc.pb.cc
            src/raft_service.cpp src/user_service.cpp)
target_include_directories(rpc PUBLIC include protos)
target_link_libraries(rpc PUBLIC gRPC::grpc++ errors)
