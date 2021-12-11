find_package(gRPC REQUIRED)
add_library(rpc src/server.cpp proto/abeille.pb.cc proto/abeille.grpc.pb.cc)
target_include_directories(rpc PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(rpc PUBLIC gRPC::grpc++ utils)
