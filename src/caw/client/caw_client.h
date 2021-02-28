#include "caw.pb.h"
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include <glog/logging.h>

#include "../../faz/client/faz_client.h"

using google::protobuf::Any;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using caw::Caw;
using caw::CawReply;
using caw::CawRequest;
using caw::FollowReply;
using caw::FollowRequest;
using caw::ProfileReply;
using caw::ProfileRequest;
using caw::ReadReply;
using caw::ReadRequest;
using caw::RegisteruserReply;
using caw::RegisteruserRequest;
using caw::Timestamp;

// This class functions as a bridge 
// between the caw cli tool that the 
// user uses and our faz server that runs 
// our functions. 
class CawClient final {
public:
  // Initialize to set up and connect Faz Server
  CawClient(const std::shared_ptr<Channel> &channel);

  // Calls the Faz server to perform the
  // Following operations

  // Registers the user in the kvstore database
  // Args: the users username string 
  // Returns: bool to indicate success / failure
  bool RegisterUser(const std::string& username);
  // Creates and saves a caw "post" in the kvstore
  // This also performs the reply functionality
  // when given a parent id
  // Args: posters username and text to post
  // Returns: bool to indicate success / failure
  bool PostCaw(const std::string& username, const std::string& text, int parent_id);
  // Follows a user
  // Args: username of person following and username of person
  //       to follow
  // Returns: bool to indicate success / failure
  bool FollowUser(const std::string& username, const std::string& to_follow);
  // Gets an entire caw thread from the database
  // and prints it out in a readable format
  // Args: Id of the caw whose sub thread user wants 
  // Returns: a string of the caw threads
  bool ReadCaw(int caw_id);
  // Gets a users following and follower lists 
  // and prints them out in readable formats
  // Args: username of user whose lists we want
  // Returns: a vector of two vectors, followers and following
  bool GetProfile(const std::string& username);

private:
  // A map linking functionName with eventType
  std::unordered_map<std::string, int> functionToEventType_;
  // An instance of the Faz Client 
  FazClient client_;

};