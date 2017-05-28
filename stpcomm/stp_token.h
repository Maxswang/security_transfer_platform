#ifndef STP_TOKEN_H_
#define STP_TOKEN_H_

#include <string>
#include <stdint.h>
#include "codec/rpc.pb.h"

class StpToken
{
public:
    static bool GenerateToken(int64_t stp_guid, int32_t group, int32_t idx, time_t expires, std::string&, rpc::StpToken&);
    
    static bool ValidateToken(const rpc::StpToken& token);
};

#endif