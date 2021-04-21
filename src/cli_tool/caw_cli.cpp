#include <gflags/gflags.h>

#include <iostream>
#include <string>

#include "../caw/client/cpp_client/caw_client.h"

DEFINE_string(registeruser, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(caw, "", "Creates a new Caw with the given text");
DEFINE_int64(reply, -1,
             "Indicates that the new caw is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_int64(read, -1, "Reads the caw thread starting at the given id");
DEFINE_bool(profile, false,
            "Gets the user’s profile of following and followers");
DEFINE_string(stream, "", "Streams all new caws containing the given hashtag");
DEFINE_string(hook, "", "Hooks the given function");
DEFINE_string(unhook, "", "Unhooks the given function");
DEFINE_bool(hookall, false, "Hooks all stored functions");
DEFINE_bool(unhookall, false, "Unhooks all stored functions");

int main(int argc, char *argv[]) {
  // Checking and handeling arguments
  if (argc < 2) {
    gflags::ShowUsageWithFlags(argv[0]);
    return 1;
  }

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // Ensuring a username is always specified
  // With exception of registeruser
  if (FLAGS_registeruser == "" && FLAGS_user == "" && FLAGS_hook == "" &&
      FLAGS_unhook == "" && FLAGS_unhookall == false && FLAGS_stream == "" &&
      FLAGS_hookall == false) {
    std::cerr << "You cannot perform any action other than hooking/unhooking "
                 "without specifying a username"
              << std::endl;
    std::cerr << "Please consider using the --user <username> flag"
              << std::endl;
    return 1;
  }

  if (FLAGS_hookall && FLAGS_unhookall) {
    std::cerr << "You cannot hook all and unhook all at the same time"
              << std::endl;
    return 1;
  }

  if (FLAGS_reply != -1 && FLAGS_caw == "") {
    std::cerr << "You cannot reply without specifying a caw" << std::endl;
    std::cerr << "Please consider using the --caw <caw text> flag" << std::endl;
    return 1;
  }

  // Error checking done, handeling arguments

  // Creating a client instance
  CawClient client;

  if (FLAGS_registeruser != "") {
    bool registerSuccess = client.RegisterUser(FLAGS_registeruser);
  } else if (FLAGS_reply != -1) {
    bool replySuccess = client.PostCaw(FLAGS_user, FLAGS_caw, FLAGS_reply);
  } else if (FLAGS_caw != "") {
    // Since we have already checked for reply
    // We know this would be a new caw and not a reply
    // According to specs, we specify a new caw using -1
    bool postSuccess = client.PostCaw(FLAGS_user, FLAGS_caw, -1);
  } else if (FLAGS_follow != "") {
    bool followSuccess = client.FollowUser(FLAGS_user, FLAGS_follow);
  } else if (FLAGS_read != -1) {
    bool readSuccess = client.ReadCaw(FLAGS_read);
  } else if (FLAGS_profile) {
    bool profileSuccess = client.GetProfile(FLAGS_user);
  } else if (FLAGS_stream != "") {
    bool streamSuccess = client.StreamHashtag(FLAGS_stream);
  } else if (FLAGS_hook != "") {
    bool hookSuccess = client.HookFunction(FLAGS_hook);
  } else if (FLAGS_unhook != "") {
    bool unhookSuccess = client.UnhookFunction(FLAGS_unhook);
  } else if (FLAGS_hookall) {
    bool hookallSuccess = client.HookAll();
  } else if (FLAGS_unhookall) {
    bool unhookallSuccess = client.UnhookAll();
  }

  return 0;
}
