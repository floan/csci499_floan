#include <mutex>
#include <string>
#include <vector>

#include "../src/key_value_store/core/KeyValueStore.h"
#include "gtest/gtest.h"

// This is a basic test to ensure the
// store actually stores in key/value pairs
TEST(KeyValueStoreTest, basic_put_pass) {
  KeyValueStore test_store;
  EXPECT_EQ(true, test_store.Put("func1", "sayHello"));
}

// Ensures that the return value is the same
// as what was stored (i.e data isnt changed)
TEST(KeyValueStoreTest, basic_get_hasResult) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ("sayHello", test_store.Get("func1")[0]);
}

// Checks to ensure the function returns
// an empty vector when key is not found
TEST(KeyValueStoreTest, basic_get_hasNoResult) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ(0, test_store.Get("func2").size());
}

// Ensures remove functionality works
TEST(KeyValueStoreTest, basic_remove_pass) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ(true, test_store.Remove("func1"));
  EXPECT_EQ(0, test_store.Get("func1").size());
}

// Returns false when key doesn't exist
// on a Remove(key) call
TEST(KeyValueStoreTest, basic_remove_fail) {
  KeyValueStore test_store;
  EXPECT_EQ(false, test_store.Remove("func1"));
}

// Makes sure Remove(key) removes the entire
// Value list instead of just one value
TEST(KeyValueStoreTest, remove_entire_mapping) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  test_store.Put("func1", "sayHi");
  test_store.Put("func1", "sayBye");
  test_store.Put("func1", "sayCool");
  test_store.Put("func1", "sayNotCool");
  EXPECT_EQ(true, test_store.Remove("func1"));
  EXPECT_EQ(false, test_store.Remove("func1"));
}

// simple one subscriber
TEST(KeyValueStoreTest, get_stream_one) {
  const std::string kMessage = "message #tag1 and rest of message";
  KeyValueStore test_store;
  std::function<bool(std::string)> f1 = [&kMessage](std::string m) {
    EXPECT_EQ(kMessage, m);
    return true;
  };
  test_store.Get("tag1", f1);
  test_store.Put("key1", kMessage);
}

// two subscribers to one key
TEST(KeyValueStoreTest, get_stream_two) {
  const std::string kMessage = "message #tag1 and rest of message";
  int count = 0;
  KeyValueStore test_store;
  std::function<bool(std::string)> f1 = [&kMessage, &count](std::string m) {
    EXPECT_EQ(kMessage, m);
    count++;
    return true;
  };
  test_store.Get("tag1", f1);
  test_store.Get("tag1", f1);
  test_store.Put("key1", kMessage);
  EXPECT_EQ(2, count);
}

// test to make sure when a callback returns false that it is removed
// from the sub map
TEST(KeyValueStoreTest, get_stream_fail) {
  const std::string kMessage = "message #tag1 and rest of message";
  int count = 0;
  KeyValueStore test_store;
  std::function<bool(std::string)> f1 = [&kMessage, &count](std::string m) {
    EXPECT_EQ(kMessage, m);
    count++;
    return false;
  };
  test_store.Get("tag1", f1);
  test_store.Put("key1", kMessage);
  test_store.Put("key2", kMessage);
  EXPECT_EQ(1, count);
}

// simple test of two separate key streams
TEST(KeyValueStoreTest, get_stream_two_subs) {
  const std::string kMessage = "message #tag1 and #tag2 and rest of message";
  int count = 0;
  KeyValueStore test_store;
  std::function<bool(std::string)> f1 = [&kMessage, &count](std::string m) {
    EXPECT_EQ(kMessage, m);
    count++;
    return true;
  };
  test_store.Get("tag1", f1);
  test_store.Get("tag2", f1);
  test_store.Put("key1", kMessage);
  EXPECT_EQ(2, count);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
