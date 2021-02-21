// This is a file that will contain the caw functions
// to be registered with, and used by Faz
#include "caw_functions.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

bool isInList(const std::vector<std::string> &list, const std::string &key) {
  for (std::string s : list) {
    if (s == key) {
      return true;
    }
  }
  return false;
}

std::vector<std::string> stringToVector(const std::string &strToConvert) {
  std::stringstream ss(strToConvert);
  std::vector<std::string> toReturn;
  std::string word;
  while (ss >> word) {
    toReturn.push_back(word);
  }
  return toReturn;
}

void makeCawFromId(Caw *caw, const std::string &caw_id,
                   KeyValueStoreInterface &kvstore) {
  std::vector<std::string> currentCaw;
  Timestamp *timestamp = new Timestamp;
  std::vector<std::string> timestamps;

  currentCaw = kvstore.Get("caw_" + caw_id);

  caw->set_username(currentCaw[0]);
  caw->set_text(currentCaw[1]);
  caw->set_id(caw_id);
  caw->set_parent_id(currentCaw[2]);
  // Construct the timestamp and set it
  timestamps = stringToVector(currentCaw[3]);
  timestamp->set_seconds(std::stol(timestamps[0]));
  timestamp->set_useconds(std::stol(timestamps[1]));
  caw->set_allocated_timestamp(timestamp);
}

Status RegisterUser(const Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore) {
  RegisteruserRequest request;
  RegisteruserReply response;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  Status status;
  if (isInList(userList, request.username())) {
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
  std::vector<std::string> all_caws = kvstore.Get("all_caws");
  Status status;
  if (!isInList(userList, request.username())) {
    status =
        Status(StatusCode::NOT_FOUND, "The provided username does not exist.");
  } else if (request.parent_id() != "" &&
             !isInList(all_caws, request.parent_id())) {
    status =
        Status(StatusCode::NOT_FOUND, "The provided parent_id does not exist.");
  } else {
    // Grab the latest id and increment
    std::string currentCawId;
    if (all_caws.size() > 0) {
      currentCawId =
          std::to_string(std::stoi(all_caws[all_caws.size() - 1]) + 1);
    } else {
      currentCawId = "1";
    }
    std::string username = request.username();
    std::string text = request.text();
    std::string parent_id = request.parent_id();
    std::string timestamp;

    auto millisec_since_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    auto sec_since_epoch =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    timestamp = std::to_string(sec_since_epoch);
    timestamp += " ";
    timestamp += std::to_string(millisec_since_epoch);
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

    for (const std::string &data : currentCaw) {
      kvstore.Put("caw_" + currentCawId, data);
    }

    // If the parent is present we want to
    // include current caw in parent's childrens
    if (parent_id != "") {
      std::vector<std::string> parent_caw;
      parent_caw = kvstore.Get("caw_" + parent_id);
      kvstore.Remove("caw_" + parent_id);
      // Updating and inserting
      parent_caw[4] += (currentCawId + " ");
      for (const std::string &data : parent_caw) {
        kvstore.Put("caw_" + parent_id, data);
      }
    }

    // We will return this caw
    Caw *toReturn = new Caw;
    makeCawFromId(toReturn, currentCawId, kvstore);

    response.set_allocated_caw(toReturn);
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
    Caw *addCawsToResponse = response.add_caws();
    makeCawFromId(addCawsToResponse, caw_id, kvstore);
    childrenCawString = currentCaw[4];
    childrenCaws = stringToVector(childrenCawString);
    // repeat process for children
    for (const std::string &id : childrenCaws) {
      // Add all the children caws to the reply stream
      addCawsToResponse = response.add_caws();
      makeCawFromId(addCawsToResponse, id, kvstore);
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
  if (!isInList(userList, request.username()) ||
      !isInList(userList, request.to_follow())) {
    status =
        Status(StatusCode::NOT_FOUND, "You have not provided valid usernames");
  } else if (request.username() == request.to_follow()) {
    status = Status(StatusCode::INVALID_ARGUMENT, "You cannot follow yourself");
  } else if (isInList(
                 kvstore.Get("caw_user_" + request.username() + "_following"),
                 request.to_follow())) {
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
  if (!isInList(userList, request.username())) {
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
