#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

// Alias for the unorderedList
typedef std::unordered_map<std::string, 
  std::vector<std::string>> hashMap; 


class KeyValueStore {
 public:      
  //Declaring constructor & destructor in .h file
  KeyValueStore() {};
  ~KeyValueStore() {};

  bool Put(std::string key, std::string value);
  std::vector<std::string> Get(std::string key);
  bool Remove(std::string key); 

 private:
  bool CheckIfExists(std::vector<std::string> store, 
    std::string value);
  hashMap kv_store_;
  std::mutex lock_;
};