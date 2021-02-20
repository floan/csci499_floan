// This is a file that will contain the caw functions
// to be registered with, and used by Faz

#include <grpcpp/grpcpp.h>
#include "caw.pb.h"

#include <string>
#include <vector>
#include <sstream>

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

std::vector<std::string>> stringToVector(std::string) {
  std::stringstream ss(string);
  std::vector<std::string> toReturn; 
  std::string word;
  while (ss >> word) {
    toReturn.push_back(word);
  }
  return toReturn;
}

Status RegisterUser(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  RegisteruserRequest request;
  RegisteruserReply response;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  Status status; 
  if (isInList(userList, request.username())) {
    status = Status(StatusCode::ALREADY_EXISTS, "The provided username already exists.");
  } else {
    kvstore.Put("caw_users", request.username());
    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}

Status PostCaw(Any& EventRequest, KeyValueStoreInterface& kvstore) {
  CawRequest request;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  Status status; 
  if (!isInList(userList, request.username())) {
    status = Status(StatusCode::NOT_FOUND, "The provided username does not exist.");
  } else {
    // all_caws stores the id in string
    // This is so we can remove caws in the future
    // If a cawid was based on the size of the caw array
    // If we stored 6 caws (ids 1-6) and then removed 1
    // The latest caw will have id 6, but id 6 pre-exists
    std::vector<std::string> all_caws = kvstore.Get("all_caws");
    std::string currentCawId = std::to_string(
      std::stoi(all_caws[all_caws.size() - 1]) + 1);
    std::string username = request.username(); 
    std::string text = request.text();
    Timestamp timestamp; 
    // Now we will begin storing the caw in the kvstore
    // First we will store the caw id 
    kvstore.Put("all_caws", currentCawId);
    // Create caw vector
    std::vector<std::string> currentCaw;
    // caw_id[0] = username
    currentCaw.push_back(username); 
    // caw_id[1] = text
    currentCaw.push_back(text);
    // caw_id[2] = parent_id;
    currentCaw.push_back("");
    // caw_id[3] = timestamp;
    currentCaw.push_back(timestamp);
    // caw_id[4] = children; 
    currentCaw.push_back("");

    kvstore.Put("caw_"+currentCawId, currentCaw);

    status = Status::OK;
  }
  return status;
}

Status ReadCaw(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  ReadRequest request; 
  Caw caw;
  ReadReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::string caw_id = request.caw_id();
  std::vector<std::string> all_caws = kvstore.Get("all_caws");
  if (!isInList(all_caws, caw_id)) {
    status = Status(StatusCode::NOT_FOUND, "The provided caw does not exist.");
  } else {
    // We will perform a dfs tree traversal 
    // To get all threads and sub threads
    std::vector<std::string> currentCaw;
    std::string childrenCawString;
    std::vector<std::string> childrenCaws;
    std::stack<std::string> dfsStack; 
    dfsStack.push(caw_id);

    // Should never get a cyclic graph 
    // So no need to keep a visited Array
    while (!dfsStack.empty()) {
      caw_id = dfsStack.top();
      dfsStack.pop();
      currentCaw = kvstore.Get("caw_" + caw_id);
      caw.set_username(currentCaw[0]);
      caw.set_text(currentCaw[1]);
      caw.set_id(caw_id);
      caw.set_parent_id(currentCaw[2]);
      caw.set_allocated_timestamp(currentCaw[3]);
      // TODO: FIGURE OUT HOW TO WRITE THIS CAW TO 
      // THE REPLY STREAM
      childrenCawString = currentCaw[4];
      childrenCaws = stringToVector(childrenCawString);
      for (int i = childrenCaws.size() - 1; i >= 0; i--) {
        dfsStack.push(childrenCaws[i]);
      }
    }
  }

}




