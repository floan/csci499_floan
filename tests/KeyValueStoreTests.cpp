#include "../src/key_value_store/core/KeyValueStore.h"

#include <mutex>
#include <string>
#include <vector>

#include "gtest/gtest.h"

TEST(KeyValueStoreTest, basic_put_pass) {
  KeyValueStore test_store;
  EXPECT_EQ(true, test_store.Put("func1", "sayHello"));
}

TEST(KeyValueStoreTest, basic_put_fail) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  // Should fail because inserting same data
  EXPECT_EQ(false, test_store.Put("func1", "sayHello"));
}

TEST(KeyValueStoreTest, basic_get_hasResult) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ("sayHello", test_store.Get("func1")[0]);
}

TEST(KeyValueStoreTest, basic_get_hasNoResult) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ(0, test_store.Get("func2").size());
}

TEST(KeyValueStoreTest, basic_remove_pass) {
  KeyValueStore test_store;
  test_store.Put("func1", "sayHello");
  EXPECT_EQ(true, test_store.Remove("func1"));
}

TEST(KeyValueStoreTest, basic_remove_fail) {
  KeyValueStore test_store;
  EXPECT_EQ(false, test_store.Remove("func1"));
}

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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}