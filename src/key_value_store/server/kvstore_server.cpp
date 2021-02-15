#include "kvstore_server.h"

#include <vector>

Status KeyValueStoreImpl::put(ServerContext* context, 
    const PutRequest* request, PutReply* response) override {
  bool result = store_.Put(request->key(), request->value());
  if (result) {
    return Status::OK;
  } else {
    return Status::ALREADY_EXISTS;
  }
}

Status KeyValueStoreImpl::get(ServerContext* context,
    ServerReaderWriter<GetReply, GetRequest>* stream) override {
  // TODO: ASK WHY WE RECIEVE A STREAM OF REQUESTS 
  // INSTEAD OF JUST ONE REQUEST
  // SINCE WE ARE ONLY SENDING ONE KEY

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
    const RemoveRequest* request, RemoveReply* response) override {
  bool result = store_.Remove(request->key());
  if (result) {
    return Status::OK;
  } else {
    return Status::NOT_FOUND;
  }
}