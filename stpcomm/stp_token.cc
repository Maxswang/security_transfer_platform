#include "stp_token.h"
#include "stputil/json/json.h"
#include <glog/logging.h>

std::string StpToken::GenerateToken(int64_t stp_guid, uint32_t group, uint32_t idx, time_t expires)
{
    Json::Value value;
    value["stp_guid"] = Json::Int64(stp_guid);
    value["group"] = Json::UInt(group);
    value["idx"] = Json::UInt(idx);
    value["expires"] = Json::Int64(expires);
    
    Json::FastWriter writer;
    return writer.write(value);
}

bool StpToken::ValidateToken(const std::string &token)
{   
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(token, root, false))
    {
        return false;
    }
    
    time_t now = time(NULL);
    
    if (now >= root["expires"].asInt64())
    {
        return false;
    }
    
    return true;
}
