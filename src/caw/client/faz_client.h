#include "faz.grpc.pb.h"
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include <glog/logging.h>

#include "../../faz/client/faz_client.h"

using google::protobuf::Any;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using faz::EventRequest;
using faz::EventReply;
using faz::HookRequest;
using faz::HookReply;
using faz::UnhookRequest;
using faz::UnhookReply;

// This class functions as a bridge 
// between the caw cli tool that the 
// user uses and our faz server that runs 
// our functions. 
class FazClient final {
public:
  // This is a constructor that takes in a channel instance
  // This channel is used to connect to the backend
  FazClient(const std::shared_ptr<Channel> &channel)
      : stub_(Greeter::NewStub(channel)){};

  Status Event(const Any request, Any* response, int eventType);
  Status HookFunction(int eventType, std::string eventFunction);
  Status UnhookFunction(int eventType);

private:
  std::unique_ptr<faz::FazService::Stub> stub_;
};