find_package(gRPC REQUIRED)
add_library(rpc src/server.cpp
            proto/abeille.pb.cc proto/abeille.grpc.pb.cc
            src/raft_service.cpp src/user_service.cpp)
target_include_directories(rpc PUBLIC include proto)
target_link_libraries(rpc PUBLIC gRPC::grpc++ errors)
