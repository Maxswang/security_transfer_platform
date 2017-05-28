#include "stpcomm/stp_token.h"
#include "codec/rpc.pb.h"

#include <iostream>
using namespace std;

int main()
{
    
    rpc::StpToken token;
    std::string key;
    StpToken::GenerateToken(1234567, 0, 0, time(NULL) + 2000,key, token);
    
    if (StpToken::ValidateToken(token))
    {
        std::cout << "valid token" << std::endl;
    }
    else
    {
        std::cout << "invalid token" << std::endl;
    }
    return 0;
}