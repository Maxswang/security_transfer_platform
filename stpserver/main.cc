#include "stp_server.h"
#include <glog/logging.h>
int main()
{
    StpServer& server = StpServer::GetInstance();
    server.AddSignalEvent(SIGINT, StpServer::QuitStpServerCallback);
    
    if (!server.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer suceed!";
    
    return 0;
}
