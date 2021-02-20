#include "caw.pb.h"
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

#include "../../key_value_store/KeyValueStoreInterface.h"

using google::protobuf::Any;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using caw::Caw;
using caw::CawReply;
using caw::CawRequest;
using caw::FollowReply;
using caw::FollowRequest;
using caw::ProfileReply;
using caw::ProfileRequest;
using caw::ReadReply;
using caw::ReadRequest;
using caw::RegisteruserReply;
using caw::RegisteruserRequest;
using caw::Timestamp;

// Helper functions
bool isInList(std::vector<std::string> list, std::string key);
std::vector<std::string> stringToVector(std::string);
Caw makeCawFromId(std::string caw_id);

// Functions to be passed into Faz
Status RegisterUser(Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore);
Status PostCaw(Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
Status ReadCaw(Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
Status FollowUser(Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
Status GetProfile(Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
