#include "../KeyValueStoreInterface.h"

#include <grpcpp/grpcpp.h>
#include "store.grpc.pb.h"

#include <string>
#include <vector>

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

class KeyValueStoreClient final : public KeyValueStoreInterface {
 public:

  KeyValueStoreClient(std::shared_ptr<Channel> channel)
    : stub_(Greeter::NewStub(channel)) {};

  bool Put(std::string key, std::string value);
  std::vector<std::string> Get(std::string key);
  bool Remove(std::string key);

 private:
  std::unique_ptr<kvstore::KeyValueStore::Stub> stub_;
};