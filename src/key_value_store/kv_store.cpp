#include "server/kvstore_server.h"

#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <gflags/gflags.h>

DEFINE_string(store, "", "Filename to store/load data");

void RunServer() {
  std::string server_address("0.0.0.0:50001");
  KeyValueStoreImpl* service;
  if (FLAGS_store != "") {
    service = new KeyValueStoreImpl(FLAGS_store);
  } else {
    service = new KeyValueStoreImpl();
  }

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
  delete service;
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  RunServer();

  return 0;
}