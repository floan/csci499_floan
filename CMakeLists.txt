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

#Setting variables
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

######################### GRPC SETUP FOR KVSTORE #############################

get_filename_component(store_proto "protos/store.proto" ABSOLUTE)
get_filename_component(store_proto_path "${store_proto}" PATH)

# Generated sources
set(store_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/store.pb.cc")
set(store_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/store.pb.h")
set(store_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/store.grpc.pb.cc")
set(store_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/store.grpc.pb.h")


add_custom_command(
      OUTPUT "${store_proto_srcs}" "${store_proto_hdrs}" "${store_grpc_srcs}" "${store_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${store_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${store_proto}"
      DEPENDS "${store_proto}")

####################### GRPC SETUP FOR KVSTORE END ############################

#######################  GRPC SETUP FOR FAZ BEGIN  ############################

get_filename_component(faz_proto "protos/faz.proto" ABSOLUTE)
get_filename_component(faz_proto_path "${faz_proto}" PATH)

# Generated sources
set(faz_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.cc")
set(faz_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.h")
set(faz_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.cc")
set(faz_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.h")

add_custom_command(
      OUTPUT "${faz_proto_srcs}" "${faz_proto_hdrs}" "${faz_grpc_srcs}" "${faz_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${faz_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${faz_proto}"
      DEPENDS "${faz_proto}")

#######################  GRPC SETUP FOR FAZ END  ############################


#########################   GRPC SETUP FOR CAW   #############################

get_filename_component(caw_proto "protos/caw.proto" ABSOLUTE)
get_filename_component(caw_proto_path "${caw_proto}" PATH)

# Generated sources
set(caw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.cc")
set(caw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.h")

add_custom_command(
      OUTPUT "${caw_proto_srcs}" "${caw_proto_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${caw_proto_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${caw_proto}"
      DEPENDS "${caw_proto}")

######################### GRPC SETUP FOR CAW END #############################

###################   GRPC SETUP FOR KVSTORE PERSIST   #######################

get_filename_component(kvstore_persist "protos/store_persistence.proto" ABSOLUTE)
get_filename_component(kvstore_persist_path "${kvstore_persist}" PATH)

# Generated sources
set(kvstore_persist_srcs "${CMAKE_CURRENT_BINARY_DIR}/store_persistence.pb.cc")
set(kvstore_persist_hdrs "${CMAKE_CURRENT_BINARY_DIR}/store_persistence.pb.h")

add_custom_command(
      OUTPUT "${kvstore_persist_srcs}" "${kvstore_persist_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${kvstore_persist_path}"
        -I "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${kvstore_persist}"
      DEPENDS "${kvstore_persist}")

################### GRPC SETUP FOR KVSTORE PERSIST END #####################

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(kvstore_dir src/key_value_store/)

add_executable(kv_store src/key_value_store/kv_store.cpp ${kvstore_dir}/server/kvstore_server.cpp ${kvstore_dir}/core/KeyValueStore.cpp ${store_proto_srcs} ${store_grpc_srcs} ${kvstore_persist_srcs})
target_link_libraries(kv_store ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gflags::gflags glog::glog)

add_executable(faz src/faz/faz.cpp src/faz/server/faz_server.cpp src/faz/client/cpp_client/faz_client.cpp src/caw/functions/caw_functions.cpp ${kvstore_dir}/client/kvstore_client.cpp ${store_proto_srcs} ${store_grpc_srcs} ${caw_proto_srcs} ${caw_grpc_srcs} ${faz_proto_srcs} ${faz_grpc_srcs})
target_link_libraries(faz ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gflags::gflags glog::glog)

add_executable(caw src/cli_tool/caw_cli.cpp src/caw/client/cpp_client/caw_client.cpp src/faz/client/cpp_client/faz_client.cpp ${faz_proto_srcs} ${faz_grpc_srcs} ${caw_proto_srcs} ${caw_grpc_srcs})
target_link_libraries(caw ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gflags::gflags glog::glog)

add_executable(kv_store_test tests/KeyValueStoreTests.cpp src/key_value_store/core/KeyValueStore.cpp)
target_link_libraries(kv_store_test gtest gtest_main)

add_executable(caw_test tests/CawFunctionTests.cpp src/key_value_store/core/KeyValueStore.cpp src/caw/functions/caw_functions.cpp ${caw_proto_srcs})
target_link_libraries(caw_test ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gtest gtest_main glog::glog)

add_executable(kv_store_persistence_test tests/KeyValueStorePersistenceTests.cpp ${kvstore_dir}/core/KeyValueStore.cpp)
target_link_libraries(kv_store_persistence_test gtest gtest_main)