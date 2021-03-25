#include "../src/key_value_store/core/KeyValueStore.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"

// This test suite tests if the core kvstore class
// Returns all data in a readable format 
// and similarly, stores all data successfully 
// protobuf functionality and file io is not tested

// This function tests the GetAllEntries 
// functionality with simple data. 
// THis function is called each time the kvstore
// is deleted in order to get data and save it to a
// file.
TEST(KVStoreGetTest, basic_test) {
  KeyValueStore store; 
  store.Put("user", "user1");
  store.Put("user", "user2");
  std::vector<std::pair<std::string, std::vector<std::string>>> storeEntries;
  storeEntries = store.GetAllEntries();
  EXPECT_EQ(storeEntries.size(), 1);
  EXPECT_EQ(storeEntries[0].first, "user");
  EXPECT_EQ(storeEntries[0].second[0], "user1");
  EXPECT_EQ(storeEntries[0].second[1], "user2");
}

// This function tests the GetAllEntries 
// functionality with multiple data entries
// The expected behaviour is for all entries 
// to be returned
TEST(KVStoreGetTest, advanced_test) {
  KeyValueStore store; 
  store.Put("user", "user1");
  store.Put("user", "user2");
  store.Put("data", "csci499");
  store.Put("data", "robust systems engineering");
  store.Put("types", "int");
  store.Remove("user");
  std::vector<std::pair<std::string, std::vector<std::string>>> storeEntries;
  storeEntries = store.GetAllEntries();
  EXPECT_EQ(storeEntries.size(), 2);
  store.Remove("types");
  storeEntries = store.GetAllEntries();
  EXPECT_EQ(storeEntries.size(), 1);
  EXPECT_EQ(storeEntries[0].first, "data");
  EXPECT_EQ(storeEntries[0].second[0], "csci499");
  EXPECT_EQ(storeEntries[0].second[1], "robust systems engineering");
}

// This function tests the load functionality
// That the kvstore will do on each instantiation
// Given a specific format, it should successfully 
// save the data in its member variables
TEST(KVStoreLoadTest, basic_test) {
  KeyValueStore store; 
  std::vector<std::pair<std::string, std::vector<std::string>>> loadEntries;
  std::pair<std::string, std::vector<std::string>> p = {"user", {"user1", "user2"}};
  loadEntries.push_back(p);
  store.LoadAllEntries(loadEntries);
  EXPECT_EQ(store.Get("user").size(), 2);
  EXPECT_EQ(store.Get("user")[0], "user1");
  EXPECT_EQ(store.Get("user")[1], "user2");
}

// This function tests the same load functionality
// with multiple data. It also tests how the kvstore
// behaves when the same data is passed in twice.
TEST(KVStoreLoadTest, advanced_test) {
  KeyValueStore store; 
  std::vector<std::pair<std::string, std::vector<std::string>>> loadEntries;
  std::pair<std::string, std::vector<std::string>> p = {"user", {"user1", "user2"}};
  std::pair<std::string, std::vector<std::string>> p2 = {"data", {"data1", "data2"}};
  // Testing double insert
  std::pair<std::string, std::vector<std::string>> p3 = {"data", {"data1", "data2"}};
  loadEntries.push_back(p);
  loadEntries.push_back(p2);
  loadEntries.push_back(p3);
  store.LoadAllEntries(loadEntries);
  EXPECT_EQ(store.Get("data").size(), 2);
  EXPECT_EQ(store.GetAllEntries().size(), 2);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}