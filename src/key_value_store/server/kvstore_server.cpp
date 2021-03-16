#include "kvstore_server.h"

#include <vector>

KeyValueStoreImpl::KeyValueStoreImpl(std::string filename) {
  filename_ = filename;
  std::ifstream ifile(filename_);
  if (ifile.fail()) {
    // If file does not exist
    // Then it is a new file to be
    // created and we want to store data to it
    // ofstream will create a non existing file
    // automatically
  } else {
    ifile.close();
    loadDataFromFile();
  }
}

Status KeyValueStoreImpl::put(ServerContext *context, const PutRequest *request,
                              PutReply *response) {
  bool putSuccessBool = store_.Put(request->key(), request->value());
  if (putSuccessBool) {
    return storeDataToFile();
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
  Status status;
  bool removeSuccessBool = store_.Remove(request->key());
  if (removeSuccessBool) {
    status = storeDataToFile();
  } else {
    status = Status(StatusCode::NOT_FOUND, "The key to remove was not found");
  }
  return status;
}

Status KeyValueStoreImpl::storeDataToFile() {
  LOG(INFO) << "Storing Data To File";
  std::ofstream ofile(filename_);
  Status status;
  if (filename_ == "") {
    // If there is no filename no point in storing
    LOG(INFO) << "No filename specified so cannot save to file";
    status = Status::OK;
  } else if (ofile.fail()) {
    status = Status(StatusCode::UNKNOWN,
                    "There was a failure in opening to output file");
    LOG(ERROR) << "Failure in opening output file";
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
    // kvstoreSnapshot. We will aquire a lock 
    // and store to file.
    const std::lock_guard<std::mutex> lock(lock_);
    bool serializeSuccessBool = snapshot.SerializeToOstream(&ofile);
    if (serializeSuccessBool) {
      status = Status::OK;
    } else {
      status = Status(StatusCode::UNKNOWN,
                      "There was a failure in storing to the file");
    }
    ofile.close();
  }
  return status;
}

void KeyValueStoreImpl::loadDataFromFile() {
  // Have already checked file validity in constructor
  LOG(INFO) << "Loading Data From File";
  std::ifstream ifile(filename_);
  kvstoreSnapshot snapshot;
  // Parses the data from file
  snapshot.ParseFromIstream(&ifile);
  std::vector<std::pair<std::string, std::vector<std::string>>> toReturn;
  // Creating a pair object for each kvstorePair
  for (const kvstorePair &p : snapshot.pairs()) {
    std::pair<std::string, std::vector<std::string>> pair;
    pair.first = p.key();
    // Creating a values vector for each "repeated values"
    for (const std::string val : p.values()) {
      pair.second.push_back(val);
    }
    toReturn.push_back(pair);
  }
  // toReturn is populated with pairs
  store_.LoadAllEntries(toReturn);
}