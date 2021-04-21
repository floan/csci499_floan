#include "faz_server.h"

Status FazServiceImpl::hook(ServerContext *context, const HookRequest *request,
                            HookReply *response) {
  int eventType = request->event_type();
  std::string functionName = request->event_function();
  Status status;
  // If the key exists
  if (caw_functions_.find(functionName) != caw_functions_.end()) {
    hooked_functions_[eventType] = functionName;
    status = Status::OK;
  } else if ((caw_stream_functions_.find(functionName) !=
              caw_stream_functions_.end())) {
    hooked_functions_[eventType] = functionName;
    status = Status::OK;
  } else {  // the key does not exist
    status = Status(StatusCode::NOT_FOUND, "The given function does not exist");
  }
  return status;
}

Status FazServiceImpl::unhook(ServerContext *context,
                              const UnhookRequest *request,
                              UnhookReply *response) {
  int eventType = request->event_type();
  Status status;
  // If the key does not exist
  if (hooked_functions_.find(eventType) == hooked_functions_.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "The given event type does not exist");
  } else {
    hooked_functions_.erase(eventType);
    status = Status::OK;
  }
  return status;
}

Status FazServiceImpl::event(ServerContext *context,
                             const EventRequest *request,
                             EventReply *response) {
  int eventType = request->event_type();
  Status status;
  if (hooked_functions_.find(eventType) == hooked_functions_.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "The given event type does not exist");
  } else {
    std::string functionName = hooked_functions_[eventType];
    // Now we will call the function using the stored
    // caw functions. Functions will modify data inside
    // the Any objects.
    // We pass the object by reference, mutable_payload() returns
    // the pointer so we dereference it before passing it in.
    std::function<Status(const Any &, Any &, KeyValueStoreInterface &)>
        eventFunction;
    eventFunction = caw_functions_[functionName];
    status = eventFunction(request->payload(), *(response->mutable_payload()),
                           kvstore_);
  }
  return status;
}

Status FazServiceImpl::streamEvent(ServerContext *context,
                                   const EventRequest *request,
                                   ServerWriter<EventReply> *response) {
  int event_type = request->event_type();
  Status status;
  if (hooked_functions_.find(event_type) == hooked_functions_.end()) {
    status =
        Status(StatusCode::NOT_FOUND, "The given event type does not exist");
  } else {
    std::string function_name = hooked_functions_[event_type];
    auto event_function = caw_stream_functions_[function_name];
    // Function loads any into event reply and writes to client.  Write returns
    // boolean based on success or failure.
    std::function<bool(Any & response)> stream_writer_function =
        [&response](Any &response_any) {
          EventReply reply;
          *(reply.mutable_payload()) = response_any;
          return response->Write(reply);
        };
    status =
        event_function(request->payload(), kvstore_, stream_writer_function);
  }
  return status;
}
