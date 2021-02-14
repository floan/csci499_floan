#include "KeyValueStore.h"

#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

// Alias for the unorderedList
typedef std::unordered_map<std::string, 
  std::vector<std::string>> hashMap; 

bool KeyValueStore::Put(std::string key, std::string value) {
  bool result; 
  lock_.lock();
  // If this key already exists in the store
  if (kv_store_.find(key) != kv_store_.end()) {
    // If user puts the same function
    if (CheckIfExists(kv_store_[key], value)) {
      result = false;
    } else {
      int size = kv_store_[key].size();
      kv_store_[key].push_back(value);
      // If the value was not inserted
      if (size >= kv_store_[key].size()) {
        result = false;
      } else {
        result = true;
      }
    }
  } else { // If they key doesn't exist
    //Create a new vector with `value`
    result = kv_store_.insert({key, {value}}).second;
  }
  lock_.unlock();
  return result; 
}

std::vector<std::string> KeyValueStore::Get(std::string key) {
  std::vector<std::string> result;
  lock_.lock();
  if (kv_store_.find(key) == kv_store_.end()) {
    result = {}; 
  } else {
    result = kv_store_[key];
  }
  lock_.unlock();
  return result;
}

bool KeyValueStore::Remove(std::string key) {
  size_t result; //Returns 1 or 0
  lock_.lock();
  result = kv_store_.erase(key);
  lock_.unlock();
  return result;
}

bool KeyValueStore::CheckIfExists(std::vector<std::string> store,
  std::string value) {
  for (std::string s : store) {
    if (s == value) {
      return true; 
    }
  }
  return false;
}
