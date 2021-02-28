// This is a file that will contain the caw functions
// to be registered with, and used by Faz
#include "caw_functions.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

std::vector<std::string> stringToVector(const std::string &strToConvert) {
  std::stringstream ss(strToConvert);
  std::vector<std::string> toReturn;
  std::string word;
  while (ss >> word) {
    toReturn.push_back(word);
  }
  return toReturn;
}

Status RegisterUser(const Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore) {
  RegisteruserRequest request;
  RegisteruserReply response;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  Status status;
  if (std::find(userList.begin(), userList.end(), request.username()) !=
      userList.end()) {
    status = Status(StatusCode::ALREADY_EXISTS,
                    "The provided username already exists.");
  } else {
    kvstore.Put("caw_users", request.username());
    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}

Status PostCaw(const Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore) {
  CawRequest request;
  CawReply response;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  Status status;
  if (std::find(userList.begin(), userList.end(), request.username()) ==
      userList.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "The provided username does not exist.");
  } else if (request.parent_id() != "" &&
             kvstore.Get("caw_" + request.parent_id()).size() == 0) {
    status = Status(StatusCode::NOT_FOUND,
                    "The provided parent caw does not exist.");
  } else {
    // Grab the latest id and increment
    std::string currentCawId;
    // If this is the first caw being created
    if (kvstore.Get("latestCawId").size() == 0) {
      currentCawId = "1";
      kvstore.Put("latestCawId", currentCawId);
    } else {
      // Get last caw id and increment by 1
      currentCawId =
          std::to_string(std::stoi(kvstore.Get("latestCawId")[0]) + 1);
      // Replace previous last caw ID with current one
      kvstore.Remove("latestCawId");
      kvstore.Put("latestCawId", currentCawId);
    }

    auto millisec_since_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    auto sec_since_epoch =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    Timestamp *timestamp = new Timestamp;
    timestamp->set_seconds(sec_since_epoch);
    timestamp->set_useconds(millisec_since_epoch);

    Caw *currentCaw = new Caw;
    currentCaw->set_username(request.username());
    currentCaw->set_text(request.text());
    currentCaw->set_parent_id(request.parent_id());
    currentCaw->set_allocated_timestamp(timestamp);
    currentCaw->set_id(currentCawId);

    std::string currentCawString;
    currentCaw->SerializeToString(&currentCawString);

    // caw_id[0] = caw
    // caw_id_children = {children vector}
    kvstore.Put("caw_" + currentCawId, currentCawString);

    // If the parent is present we want to
    // include current caw in parent's childrens
    if (request.parent_id() != "") {
      kvstore.Put("caw_" + request.parent_id() + "_children", currentCawId);
    }

    // We will return this caw
    response.set_allocated_caw(currentCaw);
    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}

// This function gets the main caw and all its subthreads
// it does not get subthreads of its subthreads (1 level bfs, not dfs)
Status ReadCaw(const Any &EventRequest, Any &EventReply,
               KeyValueStoreInterface &kvstore) {
  ReadRequest request;
  ReadReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::string caw_id = request.caw_id();
  if (kvstore.Get("caw_" + caw_id).size() == 0) {
    status = Status(StatusCode::NOT_FOUND, "The provided caw does not exist.");
  } else {
    std::string currentCawString = kvstore.Get("caw_" + caw_id)[0];
    // Add this caw to the reply stream
    Caw *addCawsToResponse = response.add_caws();
    addCawsToResponse->ParseFromString(currentCawString);
    std::vector<std::string> childrenCaws =
        kvstore.Get("caw_" + caw_id + "_children");

    // repeat process for children
    for (const std::string &id : childrenCaws) {
      // Add all the children caws to the reply stream
      currentCawString = kvstore.Get("caw_" + id)[0];
      addCawsToResponse = response.add_caws();
      addCawsToResponse->ParseFromString(currentCawString);
    }
    status = Status::OK;
  }

  EventReply.PackFrom(response);
  return status;
}

Status FollowUser(const Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore) {
  FollowRequest request;
  FollowReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  if (std::find(userList.begin(), userList.end(), request.username()) ==
          userList.end() ||
      std::find(userList.begin(), userList.end(), request.to_follow()) ==
          userList.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "You have not provided valid usernames");
  } else if (request.username() == request.to_follow()) {
    status = Status(StatusCode::INVALID_ARGUMENT, "You cannot follow yourself");
  } else if (std::find(
                 kvstore.Get("caw_user_" + request.username() + "_following")
                     .begin(),
                 kvstore.Get("caw_user_" + request.username() + "_following")
                     .end(),
                 request.to_follow()) !=
             kvstore.Get("caw_user_" + request.username() + "_following")
                 .end()) {
    status = Status(StatusCode::INVALID_ARGUMENT,
                    "You are already following the user");
  } else {
    kvstore.Put("caw_user_" + request.username() + "_following",
                request.to_follow());
    kvstore.Put("caw_user_" + request.to_follow() + "_followers",
                request.username());
    status = Status::OK;
  }
  return status;
}

Status GetProfile(const Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore) {
  ProfileRequest request;
  ProfileReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  if (std::find(userList.begin(), userList.end(), request.username()) ==
      userList.end()) {
    status = Status(StatusCode::NOT_FOUND, "Username does not exsist");
  } else {
    std::vector<std::string> followers =
        kvstore.Get("caw_user_" + request.username() + "_followers");
    std::vector<std::string> following =
        kvstore.Get("caw_user_" + request.username() + "_following");

    for (const std::string &followsMe : followers) {
      response.add_followers(followsMe);
    }
    for (const std::string &iFollow : following) {
      response.add_following(iFollow);
    }

    status = Status::OK;
  }
  EventReply.PackFrom(response);
  return status;
}
