#include "../KeyValueStoreInterface.h"

#include "store.grpc.pb.h"
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

#include <glog/logging.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

// This class wraps our core Key value store implementation
// with grpc. It processes a request in cpp syntax input to
// grpc and process the result to cpp syntax output
class KeyValueStoreClient final : public KeyValueStoreInterface {
public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)){};
  // Performs normal put functionality
  bool Put(std::string key, std::string value);
  // Performs normal Get functionality
  // returns empty if key does not exist
  std::vector<std::string> Get(std::string key);
  // Performs normal remove functionality
  bool Remove(std::string key);

private:
  // A connection to our grpc server
  std::unique_ptr<kvstore::KeyValueStore::Stub> stub_;
};