#include "faz_client.h"

#include <iostream>
#include <string>
#include <vector>

#include "../../../caw/functions/caw_functions.h"

Status FazClient::Event(Any request, Any *response, int eventType) {
  // Setting up request/response wrappers
  EventRequest eventRequest;
  EventReply eventResponse;
  // Creating grpc context variable
  ClientContext context;
  *(eventRequest.mutable_payload()) = request;
  eventRequest.set_event_type(eventType);

  Status status = stub_->event(&context, eventRequest, &eventResponse);
  *response = *eventResponse.mutable_payload();

  if (!status.ok()) {
    LOG(ERROR) << "Something went wrong";
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }

  return status;
}

Status FazClient::StreamEvent(const Any request,
                              std::function<void(EventReply)> &callback,
                              int eventType) {
  // setup vars for sending to server
  EventRequest event_request;
  ClientContext context;
  *(event_request.mutable_payload()) = request;
  event_request.set_event_type(eventType);
  std::unique_ptr<ClientReader<EventReply> > reader(
      stub_->streamEvent(&context, event_request));
  EventReply reply;
  while (reader->Read(&reply)) {
    callback(reply);
  }
  return reader->Finish();
}

Status FazClient::HookFunction(int eventType, std::string functionName) {
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

Status FazClient::UnhookFunction(int eventType) {
  UnhookRequest request;
  UnhookReply response;
  ClientContext context;

  request.set_event_type(eventType);

  Status status = stub_->unhook(&context, request, &response);

  if (!status.ok()) {
    LOG(ERROR) << "Something went wrong";
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }

  return status;
}
