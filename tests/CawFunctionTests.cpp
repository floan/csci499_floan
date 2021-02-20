#include "../src/caw/functions/caw_functions.h"
#include "../src/key_value_store/core/KeyValueStore.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"

KeyValueStore store; 
KeyValueStoreInterface& kvstore = store;

TEST(RegisterUserTest, basic_put_pass) {
  Any EventRequest; 
  Any EventReply; 
  RegisteruserRequest request; 
  request.set_username("Fayez");
  EventRequest.PackFrom(request);
  Status status = RegisterUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(kvstore.Get("caw_users")[0], "Fayez");
  EXPECT_EQ(status.ok(), Status::OK.ok());
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}