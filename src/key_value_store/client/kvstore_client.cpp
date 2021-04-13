#include "kvstore_client.h"

#include <iostream>
#include <string>
#include <vector>

bool KeyValueStoreClient::Put(const std::string &key,
                              const std::string &value) {
  PutRequest request;
  PutReply response;
  ClientContext context;
  request.set_key(key);
  request.set_value(value);

  Status status = stub_->put(&context, request, &response);

  if (status.ok()) {
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
    return false;
  }
}

std::vector<std::string> KeyValueStoreClient::Get(const std::string &key) {
  GetRequest request;
  ClientContext context;
  request.set_key(key);

  // Initializing the ClientReaderWriter
  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply>> stream(
      stub_->get(&context));

  // Writing the request to the ReaderWrtier
  stream->Write(request);
  stream->WritesDone();

  GetReply response;
  std::vector<std::string> values;
  while (stream->Read(&response)) {
    values.push_back(response.value());
  }

  Status status = stream->Finish();
  if (status.ok()) {
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }
  // Either way we return values, empty vector or not
  return values;
}

bool KeyValueStoreClient::Remove(const std::string &key) {
  RemoveRequest request;
  RemoveReply response;
  ClientContext context;
  request.set_key(key);

  Status status = stub_->remove(&context, request, &response);

  if (status.ok()) {
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
    return false;
  }
}

bool KeyValueStoreClient::Subscribe(const std::string &key,
                                    ServerWriter<SubscribeReply> *stream) {
  return true;
}

bool KeyValueStoreClient::Publish(const std::string &key,
                                  const std::string &message) {
  return true;
}
