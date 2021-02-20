#include <grpcpp/grpcpp.h>
#include "caw.pb.h"

#include <string>
#include <vector>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using google::protobuf::Any;

using caw::Timestamp;
using caw::Caw;
using caw::RegisteruserRequest;
using caw::RegisteruserReply;
using caw::CawRequest;
using caw::CawReply;
using caw::FollowRequest;
using caw::FollowReply;
using caw::ReadRequest;
using caw::ReadReply;
using caw::ProfileRequest;
using caw::ProfileReply;


class CawClient final {
 public:

  //Initialize to set up and connect Faz client
  CawClient();

  bool RegisterUser(std::string username);
  bool PostCaw(std::string username, std::string text);
  bool ReplyToCaw(std::string username, std::string text, int parent_id);
  bool FollowUser(std::string username, std::string to_follow);
  // Returns a vector of the caw thread
  std::vector<std::string> ReadCaw(int caw_id);
  // Returns a vector of two vectors, followers and following
  std::vector<std::vector<std::string>> GetProfile(std::string username); 

 private:
  //An instance of the Faz Client
};