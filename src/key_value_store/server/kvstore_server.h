#ifndef KEY_VALUE_STORE_SERVER_
#define KEY_VALUE_STORE_SERVER_

#include <grpcpp/grpcpp.h>

#include "store.grpc.pb.h"

#include "../core/KeyValueStore.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

// This class implements the server side
// functionality of the key value store
class KeyValueStoreImpl final : public kvstore::KeyValueStore::Service {
public:
  KeyValueStoreImpl() : store_(){};
  // This function performs normal put functionality
  // by unwrapping protobuf messages
  // Args: Context (for additional grpc info),
  //       PutRequest Protobuf Message containing key, value
  //       PutReply Protobuf Message, Empty
  // Returns: GRPC Status indicating success/error
  Status put(ServerContext *context, const PutRequest *request,
             PutReply *response) override;
  // This function performs normal get functionality
  // by unwrapping protobuf messages
  // Args: Context (for additional grpc info),
  //       GetRequest Protobuf Message containing key
  //       GetReply Protobuf Message that will be
  //       Populated with corresponding value
  // Returns: GRPC Status indicating success/error
  Status get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) override;
  // This function performs normal remove functionality
  // by unwrapping protobuf messages
  // Args: Context (for additional grpc info),
  //       RemoveRequest Protobuf Message containing key
  //       RemoveReply Protobuf Message, Empty
  // Returns: GRPC Status indicating success/error
  Status remove(ServerContext *context, const RemoveRequest *request,
                RemoveReply *response) override;

private:
  // Local variable from core class
  KeyValueStore store_;
};
#endif