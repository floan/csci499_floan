#ifndef FAZ_CLIENT_
#define FAZ_CLIENT_

#include <glog/logging.h>
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "faz.grpc.pb.h"

using google::protobuf::Any;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using faz::EventReply;
using faz::EventRequest;
using faz::FazService;
using faz::HookReply;
using faz::HookRequest;
using faz::UnhookReply;
using faz::UnhookRequest;

// This class functions as a bridge
// between the caw client that the cli tool
// uses and our faz server that runs
// our functions.
class FazClient final {
 public:
  // This is a constructor that takes in a channel instance
  // This channel is used to connect to the backend
  FazClient(const std::shared_ptr<Channel> &channel)
      : stub_(FazService::NewStub(channel)){};
  // This method links to the event method in the server class
  // This executes our stored function via our FaaS Faz
  // Args: A request and response of protobuf Any type. These
  //       represent the input and output from our registered function
  // Returns: A grpc Status indicating success / failure
  Status Event(Any request, Any *response, int eventType);
  // calls event function for events that need to be watched to see if there
  // will be an update (similar to a pub sub functionality).  The callback will
  // be passed an EventReply object to be read by the user of the faz client.
  Status StreamEvent(const Any request,
                     std::function<void(EventReply)> &callback, int eventType);
  // This method calls the Faz hook function. This is supposed to
  // hook a custom function to our FaaS.
  // Args: An event type that corresponds with the function call
  //       And a string that represents the function name
  // Returns: A grpc Status indicating success / failure
  Status HookFunction(int eventType, std::string eventFunction);
  // This method calls the faz unhook function. This is supposed
  // to unhook a custom function to our FaaS.
  // Args: An event type whose corresponding function we are to
  //       unhook.
  // Returns: A grpc Status indicating success / failure
  Status UnhookFunction(int eventType);

 private:
  // This is the stub that we will use to
  // connect to our Faz server
  std::unique_ptr<faz::FazService::Stub> stub_;
};
#endif
