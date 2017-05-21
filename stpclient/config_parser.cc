#include "config_parser.h"

ConfigParser &ConfigParser::GetInstance()
{
    static ConfigParser s_Instance;
    return s_Instance;
}

ConfigParser::ConfigParser()
{
    
}

ConfigParser::~ConfigParser()
{
    
}

bool ConfigParser::ParseProfile(const char *file)
{
    JsonParser parser;
	
	if (!parser.Parse(file))
		return false;
	
    svr_ip_ = parser.GetString("svr_ip", "127.0.0.1");
    svr_port_ = static_cast<int16_t>(parser.GetInteger("svr_port", 9999));
    
    path_ = parser.GetString("path", "");
    flag_ = parser.GetInteger("max_idx", 100);
	
	return true;
}


