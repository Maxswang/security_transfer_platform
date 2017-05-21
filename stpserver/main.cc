#include "stp_server.h"
#include "config_parser.h"
#include "stpcomm/stp_idx_mgr.h"
#include <glog/logging.h>
#include <errno.h>
#include <signal.h>

int main()
{
    google::InstallFailureSignalHandler();
    StpIdxMgr::GetInstance();
    ConfigParser& parser = ConfigParser::GetInstance();
    if (!parser.ParseProfile())
    {
        LOG(FATAL) << "StpServer parse config file failed!";
    }
    
    StpServer& server = StpServer::GetInstance();
    server.AddSignalEvent(SIGUSR2, StpServer::QuitStpServerCallback);
    
    if (!server.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer suceed!";
    
    return 0;
}
