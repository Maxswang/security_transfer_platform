#include "stp_client.h"
#include "config_parser.h"
#include <glog/logging.h>
int main()
{
    google::InstallFailureSignalHandler();
    ConfigParser& parser = ConfigParser::GetInstance();
    if (!parser.ParseProfile())
    {
        LOG(FATAL) << "StpClient parse config file failed!";
    }
    
    StpClient& client = StpClient::GetInstance();
    timeval tv = {30, 0};
    client.AddTimerEvent(StpClient::TimerHeartBeatCallback, tv, false);
    if (!client.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer suceed!";
    
    return 0;
}
