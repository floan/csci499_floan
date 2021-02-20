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
  Status put(ServerContext *context, const PutRequest *request,
             PutReply *response) override;
  // This function performs normal get functionality
  // by unwrapping protobuf messages
  Status get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) override;
  // This function performs normal remove functionality
  // by unwrapping protobuf messages
  Status remove(ServerContext *context, const RemoveRequest *request,
                RemoveReply *response) override;

private:
  // Local variable from core class
  KeyValueStore store_;
};