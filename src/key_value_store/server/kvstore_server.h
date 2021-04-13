#ifndef KEY_VALUE_STORE_SERVER_
#define KEY_VALUE_STORE_SERVER_

#include <glog/logging.h>
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <fstream>
#include <mutex>
#include <unordered_map>

#include "../core/KeyValueStore.h"
#include "store.grpc.pb.h"
#include "store_persistence.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::kvstorePair;
using kvstore::kvstoreSnapshot;
using kvstore::PublishReply;
using kvstore::PublishRequest;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;
using kvstore::SubscribeReply;
using kvstore::SubscribeRequest;

// This class implements the server side
// functionality of the key value store
class KeyValueStoreImpl final : public kvstore::KeyValueStore::Service {
 public:
  KeyValueStoreImpl() : store_(), filename_(""){};
  // This constructor is called when a filename is provided
  KeyValueStoreImpl(std::string filename);
  // This destructor is used to save data to a file
  // before the serverice is deleted
  ~KeyValueStoreImpl() { storeDataToFile(); }
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
  // subscribes to a new event keyword and writes to the server
  // write when someone publishes with a key of that keyword
  Status subscribe(ServerContext *context, const SubscribeRequest *request,
                   ServerWriter<SubscribeReply> *reply) override;
  // notifies all subscribers of a key and returns the message
  // found in the PublishRequest
  Status publish(ServerContext *context, const PublishRequest *request,
                 PublishReply *reply) override;

 private:
  // Local helper functions

  // This function is responsible for saving
  // Data to the file specified by filename (local variable)
  // Each time a put or remove request is performed
  // Args: None
  // Returns: GRPC Status to signify success or failure
  Status storeDataToFile();
  // This function is responsible for loading
  // Data from the file each time the server is started
  // Args: None
  // Returns: None
  void loadDataFromFile();

  // Local variable from core class
  KeyValueStore store_;
  // This variable stores the filename
  // to store kvstore data into
  // if no filename is specified it will be empty
  std::string filename_;
  // Lock enables thread safe operation
  // of storing to output file
  std::mutex lock_;
  // map of key to all subscribers of that key
  std::unordered_map<std::string, std::vector<ServerWriter<SubscribeReply> *> >
      sub_map_;
};
#endif
