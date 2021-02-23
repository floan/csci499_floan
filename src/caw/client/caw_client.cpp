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
  RegisteruserRequest registerUserRequest;
  EventRequest request; 
  EventReply response;
  Any requestPayload;
  Any responsePayload;
  ClientContext context;
  registerUserRequest.set_username(username);
  requestPayload.PackFrom(registerUserRequest);
  responsePayload.PackFrom(registerUserResponse);
  // Filling the EventRequest with info 
  request.set_allocated_payload(requestPayload);
  request.set_event_type(functionToEventType_["registeruser"]);
  // Filling the EventResponse with responsePayload;
  response.set_allocated_payload(responsePayload);
  Status status = stub_->event(&context, request, &response);
  bool registerSuccessBool; 
  if (status.ok()) {
    // Because the responsePayload will always
    // be empty we don't need to do anything
    std::cout << "User registered successfully!!" << std::endl;
    registerSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    registerSuccessBool = false;
  }
  return registerSuccessBool;
}

bool CawClient::PostCaw(const std::string& username, const std::string& text) {
  CawRequest postCawRequest;
  CawReply postCawResponse;
  EventRequest request; 
  EventReply response;
  Any requestPayload;
  Any responsePayload;
  ClientContext context;
  // Creating the caw request
  postCawRequest.set_username(username);
  postCawRequest.set_text(text);
  // Adding caw request and response into payload
  requestPayload.PackFrom(postCawRequest);
  responsePayload.PackFrom(postCawResponse);
  // Filling the EventRequest with info 
  request.set_allocated_payload(requestPayload);
  request.set_event_type(functionToEventType_["caw"]);
  Status status = stub_->event(&context, request, &response);
  bool postCawSuccessBool; 
  Caw caw; 
  if (status.ok()) {
    // Extract caw from response
    response.payload().UnpackTo(&postCawResponse);
    caw = postCawResponse.caw();
    std::cout << "Your caw was posted successfully!" << std::endl;
    std::cout << "Here's your caw information: " << std::endl;
    std::cout << "\t" << "Username: " << caw.username();
    std::cout << "\t" << "Text: " << caw.text();
    std::cout << "\t" << "Id: " << caw.id();
    std::cout << "\t" << "Parent_Id: " << caw.parent_id();
    postCawSuccessBool = true;
  } else {
    LOG(ERROR) << status.error_message();
    registerSuccessBool = false;
  }
  return registerSuccessBool;
}