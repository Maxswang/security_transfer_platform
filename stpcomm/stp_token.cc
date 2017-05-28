#include "stp_token.h"
#include "stputil/json/json.h"
#include <glog/logging.h>

bool StpToken::GenerateToken(int64_t stp_guid, int32_t group, int32_t idx, time_t expires, std::string &key, rpc::StpToken &token)
{
    token.set_expires(expires);
    token.set_group(group);
    token.set_idx(idx);
    token.set_stp_guid(stp_guid);
    token.set_key(key);
    return true;
}

bool StpToken::ValidateToken(const rpc::StpToken &token)
{
    time_t now = time(NULL);
    
    if (now >= token.expires())
    {
        return false;
    }
    
    return true;
}
