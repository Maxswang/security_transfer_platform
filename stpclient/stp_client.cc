#include "stp_client.h"
#include "config_parser.h"
#include <glog/logging.h>

StpClient& StpClient::GetInstance()
{
    static StpClient s_Instance(ConfigParser::GetInstance().svr_ip(), 
                                ConfigParser::GetInstance().svr_port());
    return s_Instance;
}

void StpClient::QuitStpServerCallback(int sig, short events, void *data)
{ 
    LOG(INFO) << "Catch the SIGINT signal, quit StpServer in one second";
    StpClient *me = reinterpret_cast<StpClient*>(data);
    timeval tv = {1, 0};
    me->StopRun(&tv);
}

void StpClient::HandleConnectionEvent(Connection *conn)
{
    
}

void StpClient::HandleReadEvent(Connection *conn)
{
    
}

void StpClient::HandleWriteEvent(Connection *conn)
{
    
}

void StpClient::HandleCloseEvent(Connection *conn, short events)
{
    
}

void StpClient::HandleProtocol_Ping(Connection *conn, rpc::S2C_Ping *msg)
{
    if (conn == NULL || msg == NULL)
        return;
    
    
}

StpClient::StpClient(const char *ip, int16_t port)
    : TcpEventClient(ip, port)
{
    
}

StpClient::~StpClient()
{
    
}

