#include <grpcpp/grpcpp.h>
#include "caw.pb.h"

#include <string>
#include <vector>

#include "../../key_value_store/KeyValueStoreInterface.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using google::protobuf::Any;

using caw::Timestamp;
using caw::Caw;
using caw::RegisteruserRequest;
using caw::RegisteruserReply;
using caw::CawRequest;
using caw::CawReply;
using caw::FollowRequest;
using caw::FollowReply;
using caw::ReadRequest;
using caw::ReadReply;
using caw::ProfileRequest;
using caw::ProfileReply;


// Helper functions
bool isInList(std::vector<std::string> list, std::string key);
std::vector<std::string> stringToVector(std::string);
Caw makeCawFromId(std::string caw_id);

// Functions to be passed into Faz 
Status RegisterUser(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore);
Status PostCaw(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore);
Status ReadCaw(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore);
Status FollowUser(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore);
Status GetProfile(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore);

