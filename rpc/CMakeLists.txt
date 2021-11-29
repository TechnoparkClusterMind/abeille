find_package(gRPC REQUIRED)
add_library(proto proto/abeille.pb.cc proto/abeille.grpc.pb.cc)
target_include_directories(proto PUBLIC proto)

add_library(rpc src/server.cpp)
target_include_directories(rpc PUBLIC include)
target_link_libraries(rpc PUBLIC utils proto gRPC::grpc++)
