#include "../KeyValueStoreInterface.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Alias for the unorderedList
typedef std::unordered_map<std::string, std::vector<std::string>> hashMap;

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
  // Performs normal remove functionality
  // Args: Key to remove from kvstore
  // Returns: boolean indicating success/failure
  bool Remove(const std::string &key);

private:
  // This is the underlying data structure
  // For our key value store
  hashMap kv_store_;
  // Lock enables thread safe operation
  // of our key value store
  std::mutex lock_;
};