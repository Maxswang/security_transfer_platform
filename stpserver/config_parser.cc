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

    svr_port_ = static_cast<int16_t>(parser.GetInteger("svr_port", 9999));
	event_thread_cnt_ = parser.GetInteger("event_thread_cnt", 3);
    
    max_group_ = parser.GetInteger("max_group", 4);
    max_idx_ = parser.GetInteger("max_idx", 100);
    path_ = parser.GetString("path", "");
    flag_ = parser.GetInteger("max_idx", 100);
    
	return true;
}


