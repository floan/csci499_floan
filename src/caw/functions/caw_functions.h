#include "caw.pb.h"
#include <google/protobuf/any.pb.h>
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
// This function checks to see if a string is in a list
bool isInList(std::vector<std::string> list, std::string key);
// This function converts a space seperated string to a list
std::vector<std::string> stringToVector(std::string strToConvert);
// This function sets caw attributes
void makeCawFromId(Caw *caw, std::string caw_id,
                   KeyValueStoreInterface &kvstore);

// Functions to be passed into Faz
// This function registers a user
Status RegisterUser(Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore);
// This function posts a caw to the kvstore
Status PostCaw(Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
// This function returns a caw from the kvstore
Status ReadCaw(Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
// This function makes one user follow another
Status FollowUser(Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
// This function gets the followers & following
// of a user
Status GetProfile(Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
