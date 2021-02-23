#include "caw.pb.h"
#include <google/protobuf/any.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

#include <glog/logging.h>

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
  CawClient(const std::shared_ptr<Channel> &channel)
    : stub_(Greeter::NewStub(channel)){};

  // Calls the Faz server to perform the
  // Following operations

  // Registers the user in the kvstore database
  // Args: the users username string 
  // Returns: bool to indicate success / failure
  bool RegisterUser(const std::string& username);
  // Creates and saves a caw "post" in the kvstore
  // Args: posters username and text to post
  // Returns: bool to indicate success / failure
  bool PostCaw(const std::string& username, const std::string& text);
  // Creates and saves a caw "reply" in the kvstore
  // Args: posters username, text to post and id of caw to reply to
  // Returns: bool to indicate success / failure
  bool ReplyToCaw(const std::string& username, const std::string& text, int parent_id);
  // Follows a user
  // Args: username of person following and username of person
  //       to follow
  // Returns: bool to indicate success / failure
  bool FollowUser(const std::string& username, const std::string& to_follow);
  // Gets an entire caw thread from the database
  // Args: Id of the caw whose sub thread user wants 
  // Returns: a string of the caw threads
  std::vector<std::string> ReadCaw(int caw_id);
  // Gets a users following and follower lists 
  // Args: username of user whose lists we want
  // Returns: a vector of two vectors, followers and following
  std::vector<std::vector<std::string>> GetProfile(const std::string& username);

private:
  // A connection to the Faz Server
  std::unique_ptr<faz::FazService::Stub> stub_;
};