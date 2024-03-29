#ifndef CAW_FUNCTIONS_
#define CAW_FUNCTIONS_

#include <glog/logging.h>
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

#include "../../key_value_store/KeyValueStoreInterface.h"
#include "caw.pb.h"

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
using caw::HashtagReply;
using caw::HashtagRequest;
using caw::ProfileReply;
using caw::ProfileRequest;
using caw::ReadReply;
using caw::ReadRequest;
using caw::RegisteruserReply;
using caw::RegisteruserRequest;
using caw::Timestamp;

// Functions to be passed into Faz
// This function registers a user
// Args: EventRequest that contains a RegisteruserRequst
//       which contains the username to register
//       EventReply that contains a RegisteruserReply
//       This is empty as success/failure is indicated on return
// Returns: Status indicating success / error message
Status RegisterUser(const Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore);
// This function posts a caw to the kvstore
// Args: EventRequest that contains a CawRequest
//       which contains caw information such as
//       text, parent_id and username of poster
//       EventReply that contains a CawReply
//       This is populated with the newly created caw object
// Returns: Status indicating success / error message
Status PostCaw(const Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
// This function returns a caw from the kvstore
// Args: EventRequest that contains a ReadRequest
//       which contains the cawid we want to start at
//       EventReply that contains a ReadReply
//       which contains a stream of caw objects
// Returns: Status indicating success / error message
Status ReadCaw(const Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore);
// This function makes one user follow another
// Args: EventRequest that contains a FollowRequest
//       which contains username and to_follow username
//       EventReply that contains a FollowReply
//       which is empty because sucess is indicated on return
// Returns: Status indicating success / error message
Status FollowUser(const Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
// This function gets the followers & following
// of a user
// Args: EventRequest that contains a ProfileRequest
//       which contains the username
//       EventReply that contains a ProfileReply
//       This is populated with a stream of following
//       and followers
// Returns: Status indicating success / error message
Status GetProfile(const Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore);
// streams Caws that contain the provided hashtag
// Args: EventRequest: that contains a TagRequesst which contains the hashtag
//       std::function: callback function to execute when new Caw contains the
//       provided hashtag
// Returns: Status indicating success / error message
Status StreamTag(const Any &EventRequest, KeyValueStoreInterface &kvstore,
                 std::function<bool(Any &)> &callback);
#endif
