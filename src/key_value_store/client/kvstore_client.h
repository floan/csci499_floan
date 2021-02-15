#include "../../../build/store.grpc.pb.h"

#include <string>
#include <vector>

#include "grpcpp/grpcpp.h"

#include "../core/KeyValueStore.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;

class KeyValueStoreClient final {
 public:

  KeyValueStoreClient(std::shared_ptr<Channel> channel)
    : stub_(Greeter::NewStub(channel)) {};

  bool put(std::string key, std::string value);
  std::vector<std::string> get(std::string key);
  bool remove(std::string key);

 private:
  std::unique_ptr<kvstore::KeyValueStore::Stub> stub_;
};