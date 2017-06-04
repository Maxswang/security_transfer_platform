#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "stputil/json_parser.h"


class ConfigParser
{
public:
    static ConfigParser& GetInstance();
	
    bool ParseProfile(const char* file = "stpclnt.json");
    
    const char* svr_ip() { return svr_ip_.c_str(); }
    int16_t svr_port() const { return svr_port_; }
    
    const std::string& path() const { return path_; }
    int flag() const { return flag_; }
    uint64_t stp_guid() const { return stp_guid_; }
    int max_idx() const { return max_idx_; }
    
	
private:
    ConfigParser();
	~ConfigParser();
    
	std::string svr_ip_;
    int16_t svr_port_;
    std::string path_;
    int flag_;
    uint64_t stp_guid_;
    int max_idx_;
};

#endif 
