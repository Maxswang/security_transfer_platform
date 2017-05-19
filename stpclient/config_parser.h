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
    
	
private:
    ConfigParser();
	~ConfigParser();
    
	std::string svr_ip_;
    int16_t svr_port_;
	
};

#endif 
