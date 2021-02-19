#include <grpcpp/grpcpp.h>

#include "faz.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using faz::HookRequest;
using faz::HookReply;
using faz::UnhookRequest;
using faz::UnhookReply;
using faz::EventRequest;
using faz::EventReply;

class FazServiceImpl final :
public faz::FazService::Service {
 public:

  //Initialize this to set up the KeyValueClient
  FazServiceImpl();

  Status hook(ServerContext* context,
   const HookRequest* request, HookReply* response) override;
  Status unhook(ServerContext* context,
    const UnhookRequest* request, UnhookReply* response) override;
  Status event(ServerContext* context,
    const EventRequest* request, EventReply* response) override;

 private:
  // Local variable from core class
  // Will include KeyValueStoreClient
  // Will include Map for hookedFunctions
  // Will include Map for unhookedFuncs
  // By default unhookedfuncs will be initialized to contain
  // All the functions related to Caw features
}; 