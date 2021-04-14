#include "KeyValueStore.h"

#include <mutex>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

bool KeyValueStore::Put(const std::string &key, const std::string &value) {
  bool result;
  const std::lock_guard<std::mutex> lock(lock_);
  // If this key already exists in the store
  if (kv_store_.find(key) != kv_store_.end()) {
    kv_store_[key].push_back(value);
    result = true;
  } else {  // If they key doesn't exist
    // Create a new vector with `value`
    result = kv_store_.insert({key, {value}}).second;
  }
  // extract tags from value using regex expression and push to tags vector
  std::string str = value;
  std::smatch match;
  std::regex expr("#[A-Za-z0-9]+");
  std::vector<std::string> tags;
  while (std::regex_search(str, match, expr)) {
    for (auto m : match) {
      tags.push_back(m.str().substr(1));
    }
    str = match.suffix().str();
  }
  // execute all callbacks for all tags with value as the parameter
  for (auto tag : tags) {
    auto sub_it = sub_map_.find(tag);
    // if sub_map_ contains callback functions for a given tag
    if (sub_it != sub_map_.end()) {
      auto &callback_vec = sub_it->second;  // vector of all callbacks for tag
      // execute callbacks with value as parameter
      for (auto it = callback_vec.begin(); it != callback_vec.end();) {
        bool success = (*it)(value);
        if (!success) {  // remove callback from map if fails
          it = callback_vec.erase(it);
        } else {
          ++it;
        }
      }
    }
  }
  // lock_guard releases the lock when the mutex goes out of scope
  return result;
}

std::vector<std::string> KeyValueStore::Get(const std::string &key) {
  std::vector<std::string> result;
  const std::lock_guard<std::mutex> lock(lock_);
  if (kv_store_.find(key) == kv_store_.end()) {
    result = {};
  } else {
    result = kv_store_[key];
  }
  return result;
}

bool KeyValueStore::Remove(const std::string &key) {
  size_t result;  // Returns 1 or 0
  const std::lock_guard<std::mutex> lock(lock_);
  result = kv_store_.erase(key);
  return result;
}

kvstorePairs KeyValueStore::GetAllEntries() {
  kvstorePairs toReturn;
  const std::lock_guard<std::mutex> lock(lock_);
  for (const std::pair<std::string, std::vector<std::string>> &pair :
       kv_store_) {
    toReturn.push_back(pair);
  }
  return toReturn;
}

void KeyValueStore::LoadAllEntries(kvstorePairs kvpairs) {
  const std::lock_guard<std::mutex> lock(lock_);
  for (const std::pair<std::string, std::vector<std::string>> &p : kvpairs) {
    kv_store_[p.first] = p.second;
  }
  // All pairs have been stored into the kv_store_ obj
}

bool KeyValueStore::Get(const std::string &tag,
                        std::function<bool(std::string)> &callback) {
  const std::lock_guard<std::mutex> lock(lock_);
  auto callback_vec = sub_map_.find(tag);
  if (callback_vec != sub_map_.end()) {  // if callback key already has subs
    callback_vec->second.push_back(callback);
  } else {
    sub_map_[tag] = {callback};
  }
  return true;
}
