BUILD_DIR=build

CMAKE_DEBUG_FLAGS=-DENABLE_TESTS=OFF -DENABLE_CPPCHECK=OFF -DENABLE_SANITIZERS=ON
CMAKE_TEST_FLAGS=${CMAKE_DEBUG_FLAGS} -DENABLE_COVERAGE=ON
MAKE_FLAGS=-j$(shell nproc)

PROTO_SRC_DIR:=rpc/proto

build: clean proto
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} all

abeille-raft: proto
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} abeille-raft

abeille-worker: proto
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} abeille-worker

abeille-client: proto
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} abeille-client

build-debug:
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	make ${MAKE_FLAGS} -C ${BUILD_DIR} all

test:
	cmake -B ${BUILD_DIR} ${CMAKE_DEBUG_FLAGS}
	scan-build make ${MAKE_FLAGS} -C ${BUILD_DIR} all test

clean:
	rm -rf ${BUILD_DIR}

proto:
	protoc -I ${PROTO_SRC_DIR} --grpc_out=${PROTO_SRC_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${PROTO_SRC_DIR}/*.proto
	protoc -I ${PROTO_SRC_DIR} --cpp_out=${PROTO_SRC_DIR}  ${PROTO_SRC_DIR}/*.proto
