#ifndef STP_TOKEN_H_
#define STP_TOKEN_H_

#include <string>
#include <stdint.h>

class StpToken
{
public:
    std::string GenerateToken(int64_t stp_guid, uint32_t group, uint32_t idx, time_t expires);
    
    bool ValidateToken(const std::string& token);
};

#endif