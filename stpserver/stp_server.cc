#include "stp_server.h"
#include "config_parser.h"
#include "stpsvr_msg_dispatcher.h"
#include "stputil/connection.h"
#include "codec/proto_msg_serialization.h"
#include "stpcomm/stp_crypto_negotiate.h"

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

bool StpServer::StartRun()
{
    if (!StpCryptoNegotiate::GetInstance().Init())
    {
        LOG(ERROR) << "stp crypto negotiate init failed!";
        return false;
    }
    
    if (!TcpEventServer::StartRun())
    {
        LOG(ERROR) << "star run tcp event server failed!";
        return false;
    }
    
    LOG(INFO) << "stp server start run over!";
    return true;
}

void StpServer::HandleConnectionEvent(Connection *conn)
{
    
}

void StpServer::HandleReadEvent(Connection *conn)
{
    if (conn == NULL)
        return;
    
    std::string pkt;
    if (conn->GetNetMessage(pkt))
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
    
    rpc::S2C_Ping rsp;
    int size = sizeof(send_buf_);
    if (SerializeToArray(rsp, send_buf_, size))
    {
        conn->SendNetMessage(send_buf_, size);
    }
   
    
    LOG(INFO) << "recv client heart beat";
}

StpServer::StpServer(int16_t port)
    : TcpEventServer(port)
{
    
}

StpServer::~StpServer()
{
    
}

