#include "server/kvstore_server.h"

#include <iostream>
#include <signal.h>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

DEFINE_string(store, "", "Filename to store/load data");


// Signal Handler function
void SignalHandler(int signal)
{
  // This prevents the program 
  // from exiting immediately
  // and so the normal delete 
  // service code WILL EXECUTE 
  // on line 52 and so the code 
  // to save state to a file is 
  // in the service destructor
}

void RunServer() {
  std::string server_address("0.0.0.0:50001");
  KeyValueStoreImpl *service;
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

  // Setup for signal handler to save
  // file to disk

  struct sigaction sa;
  sa.sa_handler = SignalHandler;
  sigemptyset(&sa.sa_mask);

  RunServer();

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    std::cout << "ehllo";
  }

  return 0;
}