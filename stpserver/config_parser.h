#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "stpcomm/json_parser.h"


class ConfigParser
{
public:
    static ConfigParser& GetInstance();
	
    bool ParseProfile(const char* file = "stpsvr.json");
 
    int16_t svr_port() const { return svr_port_; }
    int event_thread_cnt () const { return event_thread_cnt_; }
	
private:
    ConfigParser();
	~ConfigParser();
    
    int16_t svr_port_;
    int event_thread_cnt_;
	
};

#endif 
