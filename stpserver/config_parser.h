#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "stputil/json_parser.h"


class ConfigParser
{
public:
    static ConfigParser& GetInstance();
	
    bool ParseProfile(const char* file = "stpsvr.json");
 
    int16_t svr_port() const { return svr_port_; }
    int event_thread_cnt () const { return event_thread_cnt_; }
    int max_group() const { return max_group_; }
    int max_idx() const { return max_idx_; }
    const std::string& path() const { return path_; }
    int flag() const { return flag_; }
	
private:
    ConfigParser();
	~ConfigParser();
    
    int16_t svr_port_;
    int event_thread_cnt_;
    int max_group_;
    int max_idx_;
    std::string path_;
    int flag_;
	
};

#endif 
