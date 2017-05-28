#include "stp_server.h"
#include "config_parser.h"
#include "stpcomm/stp_idx_mgr.h"
#include "stputil/mysql_connection_pool.h"
#include <glog/logging.h>
#include <errno.h>
#include <signal.h>

int main()
{
    google::InstallFailureSignalHandler();
    ConfigParser& parser = ConfigParser::GetInstance();
    if (!parser.ParseProfile())
    {
        LOG(FATAL) << "StpServer parse config file failed!";
    }
    LOG(INFO) << "parse config succeed!";
    
    MySQLConnectionPool::GetInstance().InitConnectionPool(parser.db_addr(), 
                                                          parser.db_port(),
                                                          parser.db_user(),
                                                          parser.db_passwd(),
                                                          parser.database(),
                                                          1, 1);
    LOG(INFO) << "InitConnectionPool succeed!";
    
    StpServer& server = StpServer::GetInstance();
    server.AddSignalEvent(SIGUSR2, StpServer::QuitStpServerCallback);
    
    if (!server.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer suceed!";
    
    return 0;
}
