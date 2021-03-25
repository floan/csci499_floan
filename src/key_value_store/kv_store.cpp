#include "server/kvstore_server.h"

#include <iostream>
#include <csignal>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

DEFINE_string(store, "", "Filename to store/load data");

namespace kvstore_service {
  KeyValueStoreImpl *service; 
}

// Signal Handler function
void SignalHandler(int signal)
{
  // This deletes our service instance
  // Thus calling the destructor 
  // and writing all data to disk
  // it then exits the program
  delete kvstore_service::service;
  exit(signal);
}

void RunServer() {
  std::string server_address("0.0.0.0:50001");
  if (FLAGS_store != "") {
    kvstore_service::service = new KeyValueStoreImpl(FLAGS_store);
  } else {
    kvstore_service::service = new KeyValueStoreImpl();
  }

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(kvstore_service::service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // Setup for signal handler to save
  // file to disk
  signal(SIGINT, SignalHandler);

  RunServer();

  return 0;
}