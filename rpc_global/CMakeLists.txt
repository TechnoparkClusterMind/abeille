find_package(gRPC REQUIRED)
add_library(rpc_global src/server.cpp
            proto/abeille.pb.cc proto/abeille.grpc.pb.cc)

target_include_directories(rpc_global PUBLIC include proto)
target_link_libraries(rpc_global PUBLIC gRPC::grpc++ utils)
