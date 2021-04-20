// This is a file that will contain the caw functions
// to be registered with, and used by Faz
#include "caw_functions.h"

#include <sys/time.h>

#include <chrono>
#include <ctime>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

Status RegisterUser(const Any &EventRequest, Any &EventReply,
                    KeyValueStoreInterface &kvstore) {
  LOG(INFO) << "Processing RegisterUser request.";
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
  LOG(INFO) << "Processing PostCaw request.";
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

    timeval currTime;
    gettimeofday(&currTime, NULL);

    Timestamp *timestamp = new Timestamp;
    timestamp->set_seconds(currTime.tv_sec);
    timestamp->set_useconds(currTime.tv_usec);

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
  LOG(INFO) << "Processing ReadCaw request.";
  ReadRequest request;
  ReadReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::string caw_id = request.caw_id();
  if (kvstore.Get("caw_" + caw_id).size() == 0) {
    status = Status(StatusCode::NOT_FOUND, "The provided caw does not exist.");
  } else {
    std::stack<std::string> dfsStack;
    std::string currentCawString;
    Caw *addCawsToResponse;
    std::vector<std::string> childrenCaws;

    dfsStack.push(caw_id);
    while (!dfsStack.empty()) {
      caw_id = dfsStack.top();
      dfsStack.pop();
      currentCawString = kvstore.Get("caw_" + caw_id)[0];
      addCawsToResponse = response.add_caws();
      addCawsToResponse->ParseFromString(currentCawString);
      std::vector<std::string> childrenCaws =
          kvstore.Get("caw_" + caw_id + "_children");
      for (int i = childrenCaws.size() - 1; i >= 0; i--) {
        // Pushing into stack backwards because we
        // want replys to be printed chronologically
        // i.e. first reply and all its replies printed
        // first, then second reply and all its replies etc.
        dfsStack.push(childrenCaws[i]);
      }
    }
    status = Status::OK;
  }

  EventReply.PackFrom(response);
  return status;
}

Status FollowUser(const Any &EventRequest, Any &EventReply,
                  KeyValueStoreInterface &kvstore) {
  LOG(INFO) << "Processing FollowUser request.";
  FollowRequest request;
  FollowReply response;
  Status status;
  EventRequest.UnpackTo(&request);
  std::vector<std::string> userList = kvstore.Get("caw_users");
  std::vector<std::string> following =
      kvstore.Get("caw_user_" + request.username() + "_following");
  if (std::find(userList.begin(), userList.end(), request.username()) ==
          userList.end() ||
      std::find(userList.begin(), userList.end(), request.to_follow()) ==
          userList.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "You have not provided valid usernames");
  } else if (request.username() == request.to_follow()) {
    status = Status(StatusCode::INVALID_ARGUMENT, "You cannot follow yourself");
  } else if (std::find(following.begin(), following.end(),
                       request.to_follow()) != following.end()) {
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
  LOG(INFO) << "Processing GetProfile request.";
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

Status StreamTag(const Any &EventRequest, KeyValueStoreInterface &kvstore,
                 std::function<bool(Any &)> &callback) {
  LOG(INFO) << "Processing StreamTag request.";
  HashtagRequest request;
  Status status;
  EventRequest.UnpackTo(&request);
  // This callback packs up a serialized caw that is returned through kv
  // into a caw object, next into a HashtagReply, next into an Any object,
  // and finally into a an EventReply for the StreamTag callback to
  // handle. the callback provided by StreamTag will handle this Any and
  // return a boolean if it was successful
  std::function<bool(std::string)> tag_callback =
      [&callback](std::string serialized_caw) {
        Caw response_caw;
        response_caw.ParseFromString(serialized_caw);
        HashtagReply hashtag_reply;
        hashtag_reply.mutable_caw()->CopyFrom(response_caw);
        Any response;
        response.PackFrom(hashtag_reply);
        return callback(response);
      };
  bool ok = kvstore.Get(request.hashtag(),
                        tag_callback);  // loops till callback returns false
  if (ok) {
    status = Status::OK;
  } else {
    status = Status(StatusCode::INTERNAL,
                    "There was an internal error processing your request");
  }
  return status;
}
