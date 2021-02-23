#include "caw_client.h"

#include <iostream>
#include <string>
#include <vector>

CawClient::CawClient(const std::shared_ptr<Channel> &channel) {
  // Creating our connection with Faz
  stub_(FazService::NewStub(channel));
  functionToEventType_["registeruser"] = 1;
  functionToEventType_["caw"] = 2;
  functionToEventType_["read"] = 3;
  functionToEventType_["follow"] = 4;
  functionToEventType_["profile"] = 5;
}

bool CawClient::RegisterUser(std::string username) {
  // Setting up payload objects
  RegisteruserRequest register_request;
  RegisteruserReply register_response;
  // Setting up payload wrapper 
  Any requestPayload;
  Any responsePayload;
  // Setting up request/response wrappers
  EventRequest requestWrapper; 
  EventReply responseWrapper;
  // Creating grpc context variable
  ClientContext context;

  // Populating objects with data
  register_request.set_username(username);

  // Adding payload objects
  // to their type 'Any' wrappers
  requestPayload.PackFrom(register_request);
  responsePayload.PackFrom(register_response);

  // Filling the EventRequest Wrappers with info 
  requestWrapper.set_allocated_payload(requestPayload);
  requestWrapper.set_event_type(functionToEventType_["registeruser"]);

  // Filling the EventResponse wrapper with info
  responseWrapper.set_allocated_payload(responsePayload);

  // Making a call to the Faz server
  Status status = stub_->event(&context, requestWrapper, &responseWrapper);

  // Processing response
  bool registerSuccessBool; 
  if (status.ok()) {
    // Because the register_response will always
    // be empty we don't need to do anything
    std::cout << "User registered successfully!" << std::endl;
    registerSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    registerSuccessBool = false;
  }
  return registerSuccessBool;
}

bool CawClient::PostCaw(const std::string& username, const std::string& text, int parent_id) {
  CawRequest caw_request;
  CawReply caw_response;

  Any requestPayload;
  Any responsePayload;

  EventRequest requestWrapper; 
  EventReply responseWrapper;

  ClientContext context;

  caw_request.set_username(username);
  caw_request.set_text(text);
  // parent_id of -1 means this is a new caw not a reply
  if (parent_id != -1) {
    caw_request.set_parent_id(std::to_string(parent_id));
  }

  requestPayload.PackFrom(caw_request);
  responsePayload.PackFrom(caw_response);

  requestWrapper.set_allocated_payload(requestPayload);
  requestWrapper.set_event_type(functionToEventType_["caw"]);

  responseWrapper.set_allocated_payload(responsePayload);

  Status status = stub_->event(&context, requestWrapper, &responseWrapper);

  // Processing response
  bool postCawSuccessBool; 
  Caw caw; 
  if (status.ok()) {
    // Extract caw from response
    responseWrapper.payload().UnpackTo(&caw_response);
    caw = caw_response.caw();
    std::cout << "Your caw was posted successfully!" << std::endl;
    std::cout << "Here's your caw information: " << std::endl;
    std::cout << "\t" << "Username: " << caw.username() << std::endl;
    std::cout << "\t" << "Text: " << caw.text() << std::endl;
    std::cout << "\t" << "Id: " << caw.id() << std::endl;
    std::cout << "\t" << "Parent_Id: " << caw.parent_id() << std::endl;
    postCawSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    registerSuccessBool = false;
  }
  return registerSuccessBool;
}

bool CawClient::FollowUser(const std::string& username, const std::string& to_follow) {
  FollowRequest follow_request;
  FollowReply follow_response;

  Any requestPayload;
  Any responsePayload;

  EventRequest requestWrapper; 
  EventReply responseWrapper;

  ClientContext context;

  follow_request.set_username(username);
  follow_request.set_to_follow(to_follow);

  requestPayload.PackFrom(follow_request);
  responsePayload.PackFrom(follow_response);

  requestWrapper.set_allocated_payload(requestPayload);
  requestWrapper.set_event_type(functionToEventType_["follow"]);

  responseWrapper.set_allocated_payload(responsePayload);

  Status status = stub_->event(&context, requestWrapper, &responseWrapper);

  // Processing response
  bool followSuccessBool; 
  if (status.ok()) {
    // Because the follow_response will always
    // be empty we don't need to do anything
    std::cout << "User followed successfully!" << std::endl;
    followSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    followSuccessBool = false;
  }
  return followSuccessBool;
}

bool CawClient::ReadCaw(int caw_id) {
  ReadRequest read_request;
  ReadReply read_response;

  Any requestPayload;
  Any responsePayload;

  EventRequest requestWrapper; 
  EventReply responseWrapper;

  ClientContext context;

  read_request.set_caw_id(caw_id);

  requestPayload.PackFrom(read_request);
  responsePayload.PackFrom(read_response);

  requestWrapper.set_allocated_payload(requestPayload);
  requestWrapper.set_event_type(functionToEventType_["read"]);

  responseWrapper.set_allocated_payload(responsePayload);

  Status status = stub_->event(&context, requestWrapper, &responseWrapper);

  // Processing response
  bool readSuccessBool; 
  if (status.ok()) {
    responseWrapper.payload().UnpackTo(&read_response);
    std::cout << "Your request was posted successfully!" << std::endl;
    std::cout << "Here's your thread information: " << std::endl;
    for (const Caw& caw : read_response.caws()) {
      std::cout << "Your caw was posted successfully!" << std::endl;
      std::cout << "Here's your caw information: " << std::endl;
      std::cout << "\t" << "Username: " << caw.username() << std::endl;
      std::cout << "\t" << "Text: " << caw.text() << std::endl;
      std::cout << "\t" << "Id: " << caw.id() << std::endl;
      std::cout << "\t" << "Parent_Id: " << caw.parent_id() << std::endl;
      std::cout << "\t" << "==============================" << std::endl;
    }
    readSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    readSuccessBool = false;
  }
  return readSuccessBool;
}

bool CawClient::GetProfile(const std::string& username) {
  ProfileRequest profile_request;
  ProfileReply profile_response;

  Any requestPayload;
  Any responsePayload;

  EventRequest requestWrapper; 
  EventReply responseWrapper;

  ClientContext context;

  profile_request.set_username(username);

  requestPayload.PackFrom(read_request);
  responsePayload.PackFrom(read_response);

  requestWrapper.set_allocated_payload(requestPayload);
  requestWrapper.set_event_type(functionToEventType_["profile"]);

  responseWrapper.set_allocated_payload(responsePayload);

  Status status = stub_->event(&context, requestWrapper, &responseWrapper);

  // Processing response
  bool profileSuccessBool; 
  if (status.ok()) {
    responseWrapper.payload().UnpackTo(&profile_response);
    std::cout << "We have retrieved the profile!" << std::endl;
    std::cout << "Here's your information: " << std::endl;
    std::cout << "Here are users followers: " << std::endl;
    for (const std::string& follower : profile_response.followers()) {
      std::cout << "\t" << follower << std::endl;
    }
    std::cout << "Here are users following: : " << std::endl;
    for (const std::string& user : profile_response.following()) {
      std::cout << "\t" << user << std::endl;
    }
    profileSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    profileSuccessBool = false;
  }
  return profileSuccessBool;
}