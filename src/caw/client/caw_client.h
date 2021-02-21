#include "caw.pb.h"
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

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

class CawClient final {
public:
  // Initialize to set up and connect Faz client
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
  // An instance of the Faz Client
};