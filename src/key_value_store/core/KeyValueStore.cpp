#include "KeyValueStore.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Alias for the unorderedList
typedef std::unordered_map<std::string, std::vector<std::string>> hashMap;

bool KeyValueStore::Put(const std::string &key, const std::string &value) {
  bool result;
  const std::lock_guard<std::mutex> lock(lock_);
  // If this key already exists in the store
  if (kv_store_.find(key) != kv_store_.end()) {
    kv_store_[key].push_back(value);
    result = true;
  } else { // If they key doesn't exist
    // Create a new vector with `value`
    result = kv_store_.insert({key, {value}}).second;
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
  size_t result; // Returns 1 or 0
  const std::lock_guard<std::mutex> lock(lock_);
  result = kv_store_.erase(key);
  return result;
}