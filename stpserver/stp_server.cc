#include "stp_server.h"
#include "config_parser.h"
#include "stpsvr_msg_dispatcher.h"
#include "stpcomm/connection.h"

#include <glog/logging.h>

StpServer& StpServer::GetInstance()
{
    static StpServer s_Instance(ConfigParser::GetInstance().svr_port());
    return s_Instance;
}

void StpServer::QuitStpServerCallback(int sig, short events, void *data)
{ 
    LOG(INFO) << "Catch the SIGINT signal, quit StpServer in one second";
    StpServer *me = reinterpret_cast<StpServer*>(data);
    timeval tv = {1, 0};
    me->StopRun(&tv);
}

void StpServer::HandleConnectionEvent(Connection *conn)
{
    
}

void StpServer::HandleReadEvent(Connection *conn)
{
    if (conn == NULL)
        return;
    
    std::string pkt;
    if (conn->GetOneUnpackedPacket(pkt))
    {
        static StpSvrMsgDispatcher s_Dispatcher;
        s_Dispatcher.OnceDispatch(conn, pkt.data(), pkt.length());
    }
}

void StpServer::HandleWriteEvent(Connection *conn)
{
    
}

void StpServer::HandleCloseEvent(Connection *conn, short events)
{
    
}

void StpServer::HandleProtocol_Ping(Connection *conn, rpc::C2S_Ping *msg)
{
    if (conn == NULL || msg == NULL)
        return;
    
    LOG(INFO) << "recv client heart beat";
}

StpServer::StpServer(int16_t port)
    : TcpEventServer(port)
{
    
}

StpServer::~StpServer()
{
    
}

