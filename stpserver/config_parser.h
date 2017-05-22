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
    int token_expires() const { return token_expires_; }
    const std::string& db_addr() const { return db_addr_; }
    const std::string& db_user() const { return db_user_; }
    const std::string& db_passwd() const { return db_passwd_; }
    int db_port() const { return db_port_; }
    const std::string& database() const { return database_; }
	
private:
    ConfigParser();
	~ConfigParser();
    
    int16_t svr_port_;
    int event_thread_cnt_;
    int max_group_;
    int max_idx_;
    std::string path_;
    int flag_;
    int token_expires_;
    std::string db_addr_;
    std::string db_user_;
    std::string db_passwd_;
    int db_port_;
    std::string database_;

    
};

#endif 
