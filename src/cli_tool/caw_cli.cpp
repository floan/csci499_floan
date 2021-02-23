#include <iostream>
#include <string>

#include <gflags/gflags.h>

DEFINE_string(registeruser, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(caw, "", "Creates a new Caw with the given text");
DEFINE_int64(reply, -1,
             "Indicates that the new caw is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_int64(read, -1, "Reads the caw thread starting at the given id");
DEFINE_bool(profile, false,
            "Gets the user’s profile of following and followers");

int main(int argc, char *argv[]) {
  // Checking and handeling arguments
  if (argc < 2) {
    gflags::ShowUsageWithFlags(argv[0]);
    return 1;
  }

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // Ensuring a username is always specified
  // With exception of registeruser
  if (FLAGS_registeruser == "" && FLAGS_user == "") {
    std::cerr << "You cannot perform any action without specifying a username"
              << std::endl;
    std::cerr << "Please consider using the --user <username> flag"
              << std::endl;
    return 1;
  }

  if (FLAGS_reply != -1 && FLAGS_caw == "") {
    std::cerr << "You cannot reply without specifying a caw" << std::endl;
    std::cerr << "Please consider using the --caw <caw text> flag" << std::endl;
    return 1;
  }

  // Error checking done, handeling arguments

  if (FLAGS_registeruser != "") {
    // Handle Register User
  } else if (FLAGS_reply != -1) {
    // Handle Reply
  } else if (FLAGS_caw != "") {
    // Since we have already checked for reply
    // We know this would be a new caw and not a reply
  } else if (FLAGS_follow != "") {
    // Handle Follow
  } else if (FLAGS_read != -1) {
    // Handle read caw thread
  } else if (FLAGS_profile) {
    // Handle Profile
  }

  return 0;
}