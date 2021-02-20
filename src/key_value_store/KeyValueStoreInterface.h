#ifndef KEY_VALUE_STORE_INTERFACE_
#define KEY_VALUE_STORE_INTERFACE_

#include <string>
#include <vector>

class KeyValueStoreInterface {
public:
  virtual ~KeyValueStoreInterface(){};

  virtual bool Put(std::string key, std::string value) = 0;
  virtual std::vector<std::string> Get(std::string key) = 0;
  virtual bool Remove(std::string key) = 0;
};
#endif