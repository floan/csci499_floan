#include "../src/caw/functions/caw_functions.h"
#include "../src/key_value_store/core/KeyValueStore.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"

KeyValueStore store; 
KeyValueStoreInterface& kvstore = store;

// Test to see if the caw function registers
// a user properly
TEST(RegisterUserTest, basic_register_user) {
  Any EventRequest; 
  Any EventReply; 
  RegisteruserRequest request; 
  request.set_username("Fayez");
  EventRequest.PackFrom(request);
  Status status = RegisterUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(kvstore.Get("caw_users")[0], "Fayez");
  EXPECT_EQ(status.error_code(), 0);
}

// Test to see if the caw function registers
// multiple users properly 
TEST(RegisterUserTest, register_multiple_users) {
  Any EventRequest; 
  Any EventReply; 
  RegisteruserRequest request; 
  // First user pre-registered from first test
  // Registering second user
  request.set_username("Loan");
  EventRequest.PackFrom(request);
  Status status = RegisterUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(kvstore.Get("caw_users")[0], "Fayez");
  EXPECT_EQ(kvstore.Get("caw_users")[1], "Loan");
}

// Tests too see if program gives error 
// When the same user is registered
TEST(RegisterUserTest, register_same_user) {
  Any EventRequest; 
  Any EventReply; 
  RegisteruserRequest request; 
  request.set_username("Fayez");
  EventRequest.PackFrom(request);
  Status status = RegisterUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(status.error_code(), 6);
}

// TEST(PostCawTest, basic_post_caw) {
//   Any EventRequest; 
//   Any EventReply; 
//   CawRequest request;
//   CawReply response;
//   request.set_username("Fayez");
//   request.set_text("This is my first Caw");
//   EventRequest.PackFrom(request);
//   Status status = PostCaw(EventRequest, EventReply, kvstore);
//   EventReply.UnpackTo(&response);
//   Caw caw = response.caw();
//   EXPECT_EQ(status.ok(), Status::OK.ok());
//   EXPECT_EQ(caw.username(), "Fayez");
//   EXPECT_EQ(caw.text(), "This is my first Caw");
//   EXPECT_EQ(caw.parent_id(), "");
// }

// Tests the follow user feature
TEST(FollowUserTest, basic_follow_test) {
  Any EventRequest; 
  Any EventReply; 
  FollowRequest request; 
  FollowReply response; 
  request.set_username("Fayez");
  request.set_to_follow("Loan");
  EventRequest.PackFrom(request);
  Status status = FollowUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(status.error_code(), 0);
  EXPECT_EQ(kvstore.Get("caw_user_Fayez_following").size(), 1);
  EXPECT_EQ(kvstore.Get("caw_user_Fayez_following")[0], "Loan");
  EXPECT_EQ(kvstore.Get("caw_user_Fayez_followers").size(), 0);
  EXPECT_EQ(kvstore.Get("caw_user_Loan_followers")[0], "Fayez");
  EXPECT_EQ(kvstore.Get("caw_user_Loan_followers").size(), 1);
  EXPECT_EQ(kvstore.Get("caw_user_Loan_following").size(), 0);
}

// Tests following yourself
TEST(FollowUserTest, follow_yourself_test) {
  Any EventRequest; 
  Any EventReply; 
  FollowRequest request; 
  FollowReply response; 
  request.set_username("Fayez");
  request.set_to_follow("Fayez");
  EventRequest.PackFrom(request);
  Status status = FollowUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(status.error_code(), 3);
}

// Tests following the same person
TEST(FollowUserTest, follow_same_person_test) {
  Any EventRequest; 
  Any EventReply; 
  FollowRequest request; 
  FollowReply response; 
  request.set_username("Fayez");
  request.set_to_follow("Loan");
  EventRequest.PackFrom(request);
  Status status = FollowUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(status.error_code(), 3);
}

// Tests following someone who does not exist
TEST(FollowUserTest, follow_imaginary_person) {
  Any EventRequest; 
  Any EventReply; 
  FollowRequest request; 
  FollowReply response; 
  request.set_username("Fayez");
  request.set_to_follow("Fayez Loan");
  EventRequest.PackFrom(request);
  Status status = FollowUser(EventRequest, EventReply, kvstore);
  EXPECT_EQ(status.error_code(), 5);
}

// Tests the get profile function
TEST(GetProfileTest, basic_getprofile_test) {
  Any EventRequest; 
  Any EventReply; 
  ProfileRequest request; 
  ProfileReply response; 
  request.set_username("Fayez");
  EventRequest.PackFrom(request);
  Status status = GetProfile(EventRequest, EventReply, kvstore);
  EventReply.UnpackTo(&response);
  EXPECT_EQ(status.error_code(), 0);
  EXPECT_EQ(response.followers().size(), 0);
  EXPECT_EQ(response.following().size(), 1);
  EXPECT_EQ(response.following()[0], "Loan");
}

// Tests get profile with following and followers
TEST(GetProfileTest, lessBasic_getprofile_test) {
  Any EventRequest; 
  Any EventReply; 
  ProfileRequest request; 
  ProfileReply response; 
  // First we will add some followers 
  FollowRequest frequest; 
  frequest.set_username("Loan");
  frequest.set_to_follow("Fayez");
  EventRequest.PackFrom(frequest);
  Status status = FollowUser(EventRequest, EventReply, kvstore);
  // Now we will get following and followers
  request.set_username("Fayez");
  EventRequest.PackFrom(request);
  status = GetProfile(EventRequest, EventReply, kvstore);
  EventReply.UnpackTo(&response);
  EXPECT_EQ(status.error_code(), 0);
  EXPECT_EQ(response.followers().size(), 1);
  EXPECT_EQ(response.following().size(), 1);
  EXPECT_EQ(response.following()[0], "Loan");
  EXPECT_EQ(response.followers()[0], "Loan");
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}