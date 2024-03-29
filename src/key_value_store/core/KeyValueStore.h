#ifndef KEY_VALUE_STORE_
#define KEY_VALUE_STORE_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../KeyValueStoreInterface.h"

// Alias for the unorderedList
typedef std::unordered_map<std::string, std::vector<std::string>> hashMap;

// Alias for a vector of kvstore pairs. Each pair has a string (key)
// and a vector of strings (value).
typedef std::vector<std::pair<std::string, std::vector<std::string>>>
    kvstorePairs;

// This class is implements our core key value
// store functionality
class KeyValueStore : public KeyValueStoreInterface {
 public:
  // Declaring constructor & destructor in .h file
  KeyValueStore(){};
  ~KeyValueStore(){};
  // Performs normal Put functionality
  // Args: Key, value pair to store in kvstore
  // Returns: boolean indicating success/failure
  bool Put(const std::string &key, const std::string &value);
  // Performs normal Get functionality
  // Args: Key to search the kvstore
  // Returns: Value list, empty if key is nonexistent
  std::vector<std::string> Get(const std::string &key);
  // performs get functionality by subscribing to a tag, and when a
  // put request comes in with the tag, the callback function provided
  // is called.  This callback function takes in the message to be sent to
  // the subscriber and returns if the message was sent successfully
  bool Get(const std::string &tag, std::function<bool(std::string)> &);
  // Performs normal remove functionality
  // Args: Key to remove from kvstore
  // Returns: boolean indicating success/failure
  bool Remove(const std::string &key);
  // Returns all the keyvalue pairs in this table
  // Args: None
  // Returns: A vector of keyvalue pairs
  kvstorePairs GetAllEntries();
  // Stores all the keyvalue pairs into the table
  // Args: Vector containing keyvalue pairs
  // Returns: None
  void LoadAllEntries(kvstorePairs kvpairs);

 private:
  // This is the underlying data structure
  // For our key value store
  hashMap kv_store_;
  // map of tags to respective callback functions to call
  std::unordered_map<std::string, std::vector<std::function<bool(std::string)>>>
      sub_map_;
  // Lock enables thread safe operation
  // of our key value store
  std::mutex lock_;
};
#endif
