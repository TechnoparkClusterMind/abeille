BUILD_DIR=build

CMAKE_DEBUG_FLAGS=-DENABLE_TESTS=OFF -DENABLE_CPPCHECK=OFF -DENABLE_SANITIZERS=ON
CMAKE_TEST_FLAGS=${CMAKE_DEBUG_FLAGS} -DENABLE_COVERAGE=ON
MAKE_FLAGS=-j$(shell nproc)

PROTO_SRC_DIR:=rpc/protos

build: clean generate_proto
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} all

build-debug:
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} user_client main

test:
	cmake -B ${BUILD_DIR} ${CMAKE_TEST_FLAGS}
	scan-build make ${MAKE_FLAGS} -C ${BUILD_DIR} all test

clean:
	rm -rf ${BUILD_DIR}

generate_proto:
	protoc -I ${PROTO_SRC_DIR} --grpc_out=${PROTO_SRC_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${PROTO_SRC_DIR}/*.proto
	protoc -I ${PROTO_SRC_DIR} --cpp_out=${PROTO_SRC_DIR}  ${PROTO_SRC_DIR}/*.proto
