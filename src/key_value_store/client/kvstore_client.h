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
  // This is a constructor that takes in a channel instance
  // This channel is used to connect to the backend
  KeyValueStoreClient(const std::shared_ptr<Channel> &channel)
      : stub_(KeyValueStore::NewStub(channel)){};
  // Performs normal put functionality
  // Args: Key, Value pair to store in kvstore
  // Returns: boolean indicating success/failure
  bool Put(const std::string &key, const std::string &value);
  // Performs normal Get functionality
  // Args: Key to search the kvstore
  // Returns: Value list, empty if key is nonexistent
  std::vector<std::string> Get(const std::string &key);
  // Performs normal remove functionality
  // Args: Key to remove from kvstore
  // Returns: boolean indicating success/failure
  bool Remove(const std::string &key);

private:
  // A connection to our grpc server
  std::unique_ptr<kvstore::KeyValueStore::Stub> stub_;
};