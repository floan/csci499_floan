#include "caw_client.h"

#include <iostream>
#include <string>
#include <vector>

bool CawClient::RegisterUser(const std::string &username) {
  // Setting up payload objects
  RegisteruserRequest register_request;
  RegisteruserReply register_response;
  // Setting up payload wrapper
  Any request;
  // Response payload has to be dynamic
  // because it will be populated in faz
  Any *response = new Any();

  // Populating objects with data
  register_request.set_username(username);

  // Adding request object object
  // to their type 'Any' wrapper
  request.PackFrom(register_request);

  Status status =
      client_.Event(request, response, functionToEventType_["registeruser"]);

  // Processing response
  bool registerSuccessBool;
  if (status.ok()) {
    // Because the register_response will always
    // be empty we don't need to do anything
    std::cout << "User registered successfully!" << std::endl;
    registerSuccessBool = true;
  } else {
    std::cout << "There was an error in registering the user." << std::endl;
    registerSuccessBool = false;
  }
  delete response;
  return registerSuccessBool;
}

bool CawClient::PostCaw(const std::string &username, const std::string &text,
                        int parent_id) {
  CawRequest caw_request;
  CawReply caw_response;

  Any request;
  Any *response = new Any();

  caw_request.set_username(username);
  caw_request.set_text(text);
  // parent_id of -1 means this is a new caw not a reply
  if (parent_id != -1) {
    caw_request.set_parent_id(std::to_string(parent_id));
  }

  request.PackFrom(caw_request);

  Status status = client_.Event(request, response, functionToEventType_["caw"]);

  // Processing response
  bool postCawSuccessBool;
  Caw caw;
  if (status.ok()) {
    // Extract caw from response
    response->UnpackTo(&caw_response);
    caw = caw_response.caw();
    std::cout << "Your caw was posted successfully!" << std::endl;
    std::cout << "Here's your caw information: " << std::endl;
    std::cout << "\t"
              << "Username: " << caw.username() << std::endl;
    std::cout << "\t"
              << "Text: " << caw.text() << std::endl;
    std::cout << "\t"
              << "Id: " << caw.id() << std::endl;
    std::cout << "\t"
              << "Parent_Id: " << caw.parent_id() << std::endl;
    std::cout << "\t"
              << "Time created: " << caw.timestamp().seconds() << std::endl;
    postCawSuccessBool = true;
  } else {
    std::cout << "There was an error in posting your caw." << std::endl;
    postCawSuccessBool = false;
  }
  delete response;
  return postCawSuccessBool;
}

bool CawClient::FollowUser(const std::string &username,
                           const std::string &to_follow) {
  FollowRequest follow_request;
  FollowReply follow_response;

  Any request;
  Any *response = new Any();

  follow_request.set_username(username);
  follow_request.set_to_follow(to_follow);

  request.PackFrom(follow_request);

  Status status =
      client_.Event(request, response, functionToEventType_["follow"]);

  // Processing response
  bool followSuccessBool;
  if (status.ok()) {
    // Because the follow_response will always
    // be empty we don't need to do anything
    std::cout << "User followed successfully!" << std::endl;
    followSuccessBool = true;
  } else {
    std::cout << "There was an error in following the user." << std::endl;
    followSuccessBool = false;
  }
  delete response;
  return followSuccessBool;
}

bool CawClient::ReadCaw(int caw_id) {
  ReadRequest read_request;
  ReadReply read_response;

  Any request;
  Any *response = new Any();

  read_request.set_caw_id(std::to_string(caw_id));

  request.PackFrom(read_request);

  Status status =
      client_.Event(request, response, functionToEventType_["read"]);

  // Processing response
  bool readSuccessBool;
  if (status.ok()) {
    response->UnpackTo(&read_response);
    std::cout << "Here's your thread information: " << std::endl;
    for (const Caw &caw : read_response.caws()) {
      std::cout << "\t"
                << "Username: " << caw.username() << std::endl;
      std::cout << "\t"
                << "Text: " << caw.text() << std::endl;
      std::cout << "\t"
                << "Id: " << caw.id() << std::endl;
      std::cout << "\t"
                << "Parent_Id: " << caw.parent_id() << std::endl;
      std::cout << "\t"
                << "Time created: " << caw.timestamp().seconds() << std::endl;
      std::cout << "\t"
                << "==============================" << std::endl;
    }
    readSuccessBool = true;
  } else {
    std::cout << "There was an error in reading the caw." << std::endl;
    readSuccessBool = false;
  }
  delete response;
  return readSuccessBool;
}

bool CawClient::GetProfile(const std::string &username) {
  ProfileRequest profile_request;
  ProfileReply profile_response;

  Any request;
  Any *response = new Any();

  profile_request.set_username(username);

  request.PackFrom(profile_request);

  Status status =
      client_.Event(request, response, functionToEventType_["profile"]);

  // Processing response
  bool profileSuccessBool;
  if (status.ok()) {
    response->UnpackTo(&profile_response);
    std::cout << "We have retrieved the profile!" << std::endl;
    std::cout << "Here's your information: " << std::endl;
    std::cout << "Here are user's followers: " << std::endl;
    for (const std::string &follower : profile_response.followers()) {
      std::cout << "\t" << follower << std::endl;
    }
    std::cout << "Here are user's following: " << std::endl;
    for (const std::string &user : profile_response.following()) {
      std::cout << "\t" << user << std::endl;
    }
    profileSuccessBool = true;
  } else {
    std::cout << "There was an error in retrieving your profile." << std::endl;
    profileSuccessBool = false;
  }
  delete response;
  return profileSuccessBool;
}

bool CawClient::HookFunction(std::string functionName) {
  Status status =
      client_.HookFunction(functionToEventType_[functionName], functionName);
  if (status.ok()) {
    std::cout << "Your function was successfully hooked." << std::endl;
    return true;
  } else {
    std::cout << "Something went wrong when hooking your function."
              << std::endl;
    return false;
  }
}

bool CawClient::UnhookFunction(std::string functionName) {
  Status status = client_.UnhookFunction(functionToEventType_[functionName]);
  if (status.ok()) {
    std::cout << "Your function was successfully unhooked." << std::endl;
    return true;
  } else {
    std::cout << "Something went wrong when unhooking your function."
              << std::endl;
    return false;
  }
}

bool CawClient::HookAll() {
  Status status;
  for (auto entry : functionToEventType_) {
    status = client_.HookFunction(entry.second, entry.first);
    if (!status.ok()) {
      std::cout << "There was a problem hooking all functions" << std::endl;
      return false;
    }
  }
  std::cout << "Successfully hooked all functions." << std::endl;
  return true;
}

bool CawClient::UnhookAll() {
  Status status;
  for (auto entry : functionToEventType_) {
    status = client_.UnhookFunction(entry.second);
    if (!status.ok()) {
      std::cout << "There was a problem unhooking all functions" << std::endl;
      return false;
    }
  }
  std::cout << "Successfully unhooked all functions." << std::endl;
  return true;
}