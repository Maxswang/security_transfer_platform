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
    LOG(INFO) << "StpClient parse config file succeed!";
    
    StpClient& client = StpClient::GetInstance();
    timeval tv = {30, 0};
    client.AddTimerEvent(StpClient::TimerHeartBeatCallback, tv, false);
    tv.tv_sec = 10;
    client.AddTimerEvent(StpClient::CryptoNegotiateCallback, tv, false);
    
    if (!client.Init())
    {
        LOG(FATAL) << "StpClient init failed!";
    }
    LOG(INFO) << "StpClient init succeed!";
    
    if (!client.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer over!";
    
    return 0;
}
