#include "faz_server.h"

FazServiceImpl::FazServiceImpl() {
  // Initializing the kvstore client to connect
  // with its backend over grpc
  std::string target_str = "localhost:50001";
  kvstore_(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  // Initializing caw_functions_ to store the function
  // data
  caw_functions_["registeruser"] = RegisterUser;
  caw_functions_["caw"] = PostCaw;
  caw_functions_["read"] = ReadCaw;
  caw_functions_["follow"] = FollowUser;
  caw_functions_["profile"] = GetProfile;
}

Status FazServiceImpl::hook(ServerContext *context, const HookRequest *request,
                            HookReply *response) {
  int eventType = request->event_type();
  std::string functionName = request->event_function();
  Status status;
  // If the key does not exist
  if (caw_functions_.find(functionName) == caw_functions_.end()) {
    status = Status(StatusCode::NOT_FOUND, "The given function does not exist");
  } else {
    hooked_functions_[eventType] = functionName;
    status = Status::OK;
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
    status = caw_functions_[functionName](request->payload(),
                                          response->payload(), kvstore_);
  }
  return status;
}