#include "faz.grpc.pb.h"

#include <unordered_map>

#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include "../../caw/functions/caw_functions.h"
#include "../../key_value_store/KeyValueStoreInterface.h"
#include "../../key_value_store/client/kvstore_client.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using faz::EventReply;
using faz::EventRequest;
using faz::HookReply;
using faz::HookRequest;
using faz::UnhookReply;
using faz::UnhookRequest;

// This class implements the server side
// functionality of Faz
class FazServiceImpl final : public faz::FazService::Service {
public:
  // Initialize this to set up the KeyValueClient
  // which we will use to communicate with the server
  // side logic of the keyvaluestore
  FazServiceImpl();

  // This function 'hooks' a function that can
  // then be called for use by the Faz client
  // Args: Context (for additional grpc info),
  //       HookRequest Protobuf Message containing
  //       an int to represent the event type
  //       and a string representing function name
  //       HookReply Protobuf Message, Empty
  // Returns: GRPC Status indicating success/error
  Status hook(ServerContext *context, const HookRequest *request,
              HookReply *response) override;
  // This function 'unhooks' a function so that it
  // may no longer be called by the Faz client
  // Args: Context (for additional grpc info),
  //       UnhookRequest Protobuf Message containing
  //       an int to represent the event type
  //       to remove
  //       UnhookReply Protobuf Message, Empty
  // Returns: GRPC Status indicating success/error
  Status unhook(ServerContext *context, const UnhookRequest *request,
                UnhookReply *response) override;
  // This function calls on of the 'hooked' functions
  // Args: Context (for additional grpc info),
  //       EventRequest Protobuf Message containing
  //       an int to represent the event type and a
  //       payload containing any input data the
  //       function may need
  //       EventReply Protobuf Message containing
  //       data produced by the hooked function call
  // Returns: GRPC Status indicating success/error
  Status event(ServerContext *context, const EventRequest *request,
               EventReply *response) override;

private:
  // This is a string -> function mapping of
  // caw functions.
  std::unordered_map<std::string, std::function<Status(
                                      Any &, Any &, KeyValueStoreInterface &)>>
      caw_functions_;
  // This is an int to string mapping of hooked functions
  // We will register an event type with a string that will
  // correspond to a function name in caw_functions.
  // We will not directly store functions in this map
  std::unordered_map<int, std::string> hooked_functions_;
  // This is they KeyValueStore that Faz will use
  // to store function data
  KeyValueStoreClient kvstore_;
};