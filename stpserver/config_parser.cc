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
    token_expires_ = parser.GetInteger("token_expires", 7200);
    db_addr_ = parser.GetString("db_addr", "127.0.0.1");
    db_user_ = parser.GetString("db_user", "root");
    db_passwd_ = parser.GetString("db_passwd", "root");
    db_port_ = parser.GetInteger("db_port", 3306);
    database_ = parser.GetString("database", "stp");
    
	return true;
}


