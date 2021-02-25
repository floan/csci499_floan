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
  virtual bool Remove(const std::string &key) = 0;
};