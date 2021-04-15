#ifndef KEY_VALUE_STORE_INTERFACE_
#define KEY_VALUE_STORE_INTERFACE_

#include <functional>
#include <string>
#include <vector>

// This will be used for easier testing
// Will not have to test our GRPC kvstore
// client, instead can pass in a local
// cpp class instance for testing if
// both of them inherit from this interface
class KeyValueStoreInterface {
 public:
  virtual ~KeyValueStoreInterface(){};

  virtual bool Put(const std::string &key, const std::string &value) = 0;
  virtual std::vector<std::string> Get(const std::string &key) = 0;
  // TODO: potentially add const to callback and make its argument by reference
  // and const as well
  virtual bool Get(const std::string &tag,
                   std::function<bool(std::string)>& callback) = 0;
  virtual bool Remove(const std::string &key) = 0;
};
#endif
