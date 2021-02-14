cmake_minimum_required(VERSION 3.13)
project(csci499_floan)

# Setting up Cpp
set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Declaring third party resources
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG       release-1.10.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

FetchContent_Declare(
        gflags
        GIT_REPOSITORY https://github.com/gflags/gflags.git
        GIT_TAG       v2.2.2
)
FetchContent_MakeAvailable(gflags)

FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG       v0.4.0
)
FetchContent_MakeAvailable(glog)

FetchContent_Declare(
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_TAG       v1.35.0
)
FetchContent_MakeAvailable(grpc)

# Setting up GRPC

get_filename_component(store_proto "protos/store.proto" ABSOLUTE)
get_filename_component(store_proto_path "${store_proto}" PATH)

# Generated sources
set(store_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/store.pb.cc")
set(store_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/store.pb.h")
set(store_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/store.grpc.pb.cc")
set(store_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/store.grpc.pb.h")

#Setting variables
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

add_custom_command(
      OUTPUT "${store_proto_srcs}" "${store_proto_hdrs}" "${store_grpc_srcs}" "${store_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${store_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${store_proto}"
      DEPENDS "${store_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


# Caw will have no .grpc lines and remove --grpc_out



add_executable(caw_cli src/cli_tool/caw_cli.cpp)
target_link_libraries(caw_cli gflags::gflags)

add_executable(kv_store src/key_value_store/kv_store.cpp ${store_proto_srcs} ${store_grpc_srcs})
target_link_libraries(kv_store ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gflags::gflags glog::glog)

add_executable(kv_store_test tests/KeyValueStoreTests.cpp src/key_value_store/core/KeyValueStore.cpp)
target_link_libraries(kv_store_test gtest gtest_main)

