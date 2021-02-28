#include "faz_client.h"

#include <iostream>
#include <string>
#include <vector>

Status FazClient::Event(const Any request, Any* response, int eventType) {
  // Setting up request/response wrappers
  EventRequest eventRequest; 
  EventReply eventResponse;
  // Creating grpc context variable
  ClientContext context;
  eventRequest.set_allocated_payload(&request);
  eventRequest.set_event_type(eventType);

  *(eventResponse.mutable_payload()) = *response; 

  Status status = stub_->event(&context, eventRequest, &eventResponse);

  return status; 
}