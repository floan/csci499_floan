#include "kvstore_server.h"

#include <vector>

KeyValueStoreImpl::KeyValueStoreImpl(std::string filename) {
  filename_ = filename;
  std::ifstream ifile(filename_);
  if (ifile.is_open()) {
    loadDataFromFile();
  }
  // If file does not exist 
  // Then it is a new file to be 
  // created and we want to store data to it
  // ofstream will create a non existing file 
  // automatically
  ifile.close();
}

Status KeyValueStoreImpl::put(ServerContext *context, const PutRequest *request,
                              PutReply *response) {
  bool putSuccessBool = store_.Put(request->key(), request->value());
  if (putSuccessBool) {
    return Status::OK;
  } else {
    return Status(StatusCode::UNKNOWN, "Sorry your operation failed");
  }
}

Status
KeyValueStoreImpl::get(ServerContext *context,
                       ServerReaderWriter<GetReply, GetRequest> *stream) {
  GetRequest request;
  while (stream->Read(&request)) {
    // Get the value vector and store it in values
    std::vector<std::string> values(store_.Get(request.key()));
    for (const std::string &s : values) {
      GetReply response;
      response.set_value(s);
      stream->Write(response);
    }
  }
  return Status::OK;
}

Status KeyValueStoreImpl::remove(ServerContext *context,
                                 const RemoveRequest *request,
                                 RemoveReply *response) {
  bool removeSuccessBool = store_.Remove(request->key());
  if (removeSuccessBool) {
    return Status::OK;
  } else {
    return Status(StatusCode::NOT_FOUND, "The key to remove was not found");
  }
}

Status KeyValueStoreImpl::storeDataToFile() {
  std::ofstream ofile(filename_);
  Status status;
  if (ofile.fail()) {
    status = Status(StatusCode::UNKNOWN, "There was a failure in opening to output file");
  } else {
    kvstoreSnapshot snapshot; 
    // Iterate over every pair and store it 
    // in a protobuf message 
    // store that in a wrapper message (kvstoreSnapshot)
    // to then store to a file
    std::vector<std::pair<std::string, std::vector<std::string>>> allPairs;
    allPairs = store_.GetAllEntries();
    for (std::pair<std::string, std::vector<std::string>> p : allPairs) {
      kvstorePair pair; 
      pair.set_key(p.first);
      // Iterate over each value string 
      // and store it in the pair message
      for (std::string value : p.second) {
        *pair.add_values() = value;
      }
      // Now store the pair message to the snapshot
      snapshot.add_pairs()->CopyFrom(pair);
    }
    // All the kvstorePairs are now stored in the 
    // kvstoreSnapshot. Now we will store to file
    bool serializeSuccessBool = snapshot.SerializeToOstream(&ofile);
    if (serializeSuccessBool) {
      status = Status::OK;
    } else {
      status = Status(StatusCode::UNKNOWN, "There was a failure in storing to the file");
    }
    ofile.close();
  }
  return status;
}


Status KeyValueStoreImpl::loadDataFromFile() {

}