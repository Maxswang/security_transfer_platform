#include "stp_client.h"
#include <glog/logging.h>
int main()
{
    StpClient& client = StpClient::GetInstance();
    
    if (!client.StartRun())
    {
        LOG(FATAL) << "Start run StpServer failed!";
    }
    
    LOG(INFO) << "Start run StpServer suceed!";
    
    return 0;
}
