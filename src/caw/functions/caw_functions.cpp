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

Caw makeCawFromId(std::string caw_id) {
  Caw caw;
  std::vector<std::string> currentCaw;
  Timestamp timestamp;
  std::vector<std::string> timestamps;
  
  currentCaw = kvstore.Get("caw_" + caw_id);

  caw.set_username(currentCaw[0]);
  caw.set_text(currentCaw[1]);
  caw.set_id(caw_id);
  caw.set_parent_id(currentCaw[2]);
  // Construct the timestamp and set it
  timestamps = stringToVector(currentCaw[3]);
  timestamp.set_seconds(timestamps[0]);
  timestamp.set_useconds(timestamps[1]);
  caw.set_allocated_timestamp(timestamp);
  return caw; 
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

Status PostCaw(Any& EventRequest, Any& EventReply KeyValueStoreInterface& kvstore) {
  CawRequest request;
  CawReply response;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  std::vector<std::string> all_caws = kvstore.Get("all_caws");
  Status status; 
  if (!isInList(userList, request.username())) {
    status = Status(StatusCode::NOT_FOUND, "The provided username does not exist.");
  } else if (request.parent_id() != "" && !isInList(all_caws, request.parent_id())) {
    status = Status(StatusCode::NOT_FOUND, "The provided parent_id does not exist.");
  } else {
    // Grab the latest id and increment
    std::string currentCawId = std::to_string(
      std::stoi(all_caws[all_caws.size() - 1]) + 1);
    std::string username = request.username(); 
    std::string text = request.text();
    std::string parent_id = request.parent_id();
    Timestamp timestamp; 
    // TODO: FIGURE OUT HOW TO GET TIMESTAMP
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
    currentCaw.push_back(parent_id);
    // caw_id[3] = timestamp;
    currentCaw.push_back(timestamp);
    // caw_id[4] = children; 
    currentCaw.push_back("");

    kvstore.Put("caw_" + currentCawId, currentCaw);

    // If the parent is present we want to 
    // include current caw in parent's childrens
    if (parent_id != "") {
      std::vector<std::string> parent_caw;
      parent_caw = kvstore.Get("caw_" + parent_id);
      kvstore.Remove("caw_" + parent_id);
      // Updating and inserting
      parent_caw[4] += (currentCawId + " ");
      kvstore.Put("caw_" + parent_id, parent_caw);
    }

    status = Status::OK;
  }
  return status;
}

// This function gets the main caw and all its subthreads
// it does not get subthreads of its subthreads (1 level bfs, not dfs)
Status ReadCaw(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  ReadRequest request; 
  ReadReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::string caw_id = request.caw_id();
  std::vector<std::string> all_caws = kvstore.Get("all_caws");

  if (!isInList(all_caws, caw_id)) {
    status = Status(StatusCode::NOT_FOUND, "The provided caw does not exist.");
  } else {
    // We will perform a level one bfs traversal 
    // To get child threads
    std::vector<std::string> currentCaw;
    std::string childrenCawString;
    std::vector<std::string> childrenCaws;
    
    currentCaw = kvstore.Get("caw_" + caw_id);
    
    // Add this caw to the reply stream
    response.add_caws(makeCawFromId(caw_id));
    childrenCawString = currentCaw[4];
    childrenCaws = stringToVector(childrenCawString);
    // repeat process for children
    for (std::string id : childrenCaws) {
      // Add all the children caws to the reply stream
      response.add_caws(makeCawFromId(id));
    }
    status = Status::OK;
  }

  EventReply.PackFrom(response);
  return status;
}

Status FollowUser(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  FollowRequest request; 
  FollowReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  if (!isInList(userList, request.username()) ||
    !isInList(userList, request.to_follow())) {
    status = Status(StatusCode::NOT_FOUND, "You have not provided valid usernames");
  } else if (request.username() == request.to_follow()) {
    status = Status(StatusCode::INVALID_ARGUMENT, "You cannot follow yourself");
  } else if (isInList(kvstore.Get("caw_user_" + request.username() + "_following"),
    request.to_follow())) {
    status = Status(StatusCode::INVALID_ARGUMENT, "You are already following the user");
  } else {
    kvstore.Put("caw_user_" + request.username() + "_following", request.to_follow());
    kvstore.Put("caw_user_" + request.to_follow() + "_followers", request.username());
    status = Status::OK;
  }
  return status;
}

Status GetProfile(Any& EventRequest, Any& EventReply, KeyValueStoreInterface& kvstore) {
  ProfileRequest request; 
  ProfileReply response; 
  Status status; 
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  if (!isInList(userList, request.username())) {
    status = Status(StatusCode::NOT_FOUND, "Username does not exsist");
  } else {
    std::vector<std::string>> followers = kvstore.Get("caw_user_" + request.username() + "_followers");
    std::vector<std::string>> following = kvstore.Get("caw_user_" + request.username() + "_following");

    for (std::string followsMe : followers) {
      response.add_followers(followsMe);
    }
    for (std::string iFollow : following) {
      response.add_following(iFollow);
    }

    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}





