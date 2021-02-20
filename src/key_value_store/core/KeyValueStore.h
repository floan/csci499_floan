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
  bool Put(std::string key, std::string value);
  // Performs normal Get functionality
  // returns empty if key does not exist
  std::vector<std::string> Get(std::string key);
  // Performs normal remove functionality
  bool Remove(std::string key);

private:
  // This is the underlying data structure
  // For our key value store
  hashMap kv_store_;
  // Lock enables thread safe operation
  // of our key value store
  std::mutex lock_;
};