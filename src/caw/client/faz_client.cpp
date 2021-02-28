#include "faz_client.h"

#include <iostream>
#include <string>
#include <vector>

Status FazClient::Event(const Any request, Any *response, int eventType) {
  // Setting up request/response wrappers
  EventRequest eventRequest;
  EventReply eventResponse;
  // Creating grpc context variable
  ClientContext context;
  eventRequest.set_allocated_payload(&request);
  eventRequest.set_event_type(eventType);

  *(eventResponse.mutable_payload()) = *response;

  Status status = stub_->event(&context, eventRequest, &eventResponse);

  if (!status.ok()) {
    LOG(ERROR) << "Something went wrong";
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }

  return status;
}

Status FazClient::Hook(int eventType, std::string functionName) {
  HookRequest request;
  HookReply response;
  ClientContext context;

  request.set_event_type(eventType);
  request.set_event_function(functionName);

  Status status = stub_->hook(&context, request, &response);

  if (!status.ok()) {
    LOG(ERROR) << "Something went wrong";
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }

  return status;
}

Status FazClient::Unhook(int eventType) {
  UnhookRequest request;
  UnhookReply response;
  ClientContext context;

  request.set_event_type(eventType);

  Status status = stub_->hook(&context, request, &response);

  if (!status.ok()) {
    LOG(ERROR) << "Something went wrong";
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }

  return status;
}