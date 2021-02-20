#include <grpcpp/grpcpp.h>

#include "store.grpc.pb.h"

#include "../core/KeyValueStore.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;

class KeyValueStoreImpl final :
public kvstore::KeyValueStore::Service {
 public:

  KeyValueStoreImpl() : store_() {};

  Status put(ServerContext* context, 
    const PutRequest* request, PutReply* response) override;
  Status get(ServerContext* context,
    ServerReaderWriter<GetReply, GetRequest>* stream) override;
  Status remove(ServerContext* context,
    const RemoveRequest* request, RemoveReply* response) override;

 private:
  // Local variable from core class
  KeyValueStore store_; 
};