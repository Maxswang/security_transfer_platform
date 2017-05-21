#include "stpcomm/stp_token.h"

#include <iostream>
using namespace std;

int main()
{
    
    std::string token = StpToken::GenerateToken(1234567, 0, 0, time(NULL) + 2000);
    std::cout << token << std::endl;
    
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