// This is a file that will contain the caw functions
// to be registered with, and used by Faz

#include <grpcpp/grpcpp.h>
#include "caw.pb.h"

#include <string>
#include <vector>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using grpc::Protobuf::Any;

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

bool isInList(std::vector<std::string> list, std::string key) {
  for (std::string s : list) {
    if (s == key) {
      return true;
    }
  }
  return false;
}

Status RegisterUser(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  RegisteruserRequest request;
  RegisteruserReply response;
  EventRequest.UnpackTo(&request);
  if (isInList(userList, request.username())) {
    status = Status(StatusCode::ALREADY_EXISTS, "The provided username already exists.");
  } else {
    kvstore.Put("caw_users", request.username());
    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}

