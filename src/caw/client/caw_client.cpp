#include "caw_client.h"

#include <iostream>
#include <string>
#include <vector>

bool CawClient::RegisterUser(std::string username) {
  RegisteruserRequest request;
  RegisteruserReply response;
  request.set_username(username);
  
}