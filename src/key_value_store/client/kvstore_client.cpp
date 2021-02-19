#include "kvstore_client.h"

#include <iostream>
#include <vector>
#include <string>

bool KeyValueStoreClient::Put(std::string key, std::string value) {
  PutRequest request; 
  PutReply response; 
  ClientContext context;
  request.set_key(key);
  request.set_value(value);

  //TODO: ASK WHY THE REQ OBJ IS SENT INSTEAD OF PTR
  Status status = stub_->put(&context, request, &response);

  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
      << std::endl;
    return false;
  }
}

std::vector<std::string> KeyValueStoreClient::Get(std::string key) {
  GetRequest request; 
  ClientContext context; 
  request.set_key(key);

  // Initializing the ClientReaderWriter
  std::shared_ptr<ClientReaderWriter<GetReply, GetRequest>> stream(
    stub_->get(&context));

  // Writing the request to the ReaderWrtier
  stream->Write(request);
  stream->WritesDone(); 

  GetRequest response;
  std::vector<std::string> values;
  while (stream->Read(&response)) {
    values.push_back(response.value());
  }

  Status status = stream->Finish(); 
  if (status.ok()) {
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
      << std::endl;
  }
  // Either way we return values, empty vector or not
  return values; 
}

bool KeyValueStoreClient::Remove(std::string key) {
  RemoveRequest request; 
  RemoveReply response; 
  ClientContext context;
  request.set_key(key);

  Status status = stub_->put(&context, request, &response);

  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
      << std::endl;
    return false;
  }
}