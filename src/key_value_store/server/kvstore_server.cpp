#include "kvstore_server.h"

#include <vector>

Status KeyValueStoreImpl::put(ServerContext* context, 
    const PutRequest* request, PutReply* response) {
  bool result = store_.Put(request->key(), request->value());
  if (result) {
    return Status::OK;
  } else {
    return Status(StatusCode::UNKNOWN, "Sorry your operation failed");
  }
}

Status KeyValueStoreImpl::get(ServerContext* context,
    ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest request;
  while (stream->Read(&request)) {
    // Get the value vector and store it in values
    std::vector<std::string> values(store_.Get(request.key()));
    for (std::string s : values) {
      GetReply response;
      response.set_value(s);
      stream->Write(response);
    }
  }
  return Status::OK;
}

Status KeyValueStoreImpl::remove(ServerContext* context,
    const RemoveRequest* request, RemoveReply* response) {
  bool result = store_.Remove(request->key());
  if (result) {
    return Status::OK;
  } else {
    return Status(StatusCode::NOT_FOUND, "The key to remove was not found");
  }
}