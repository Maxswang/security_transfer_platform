#include "stp_client.h"
#include "config_parser.h"
#include "stpclnt_msg_dispatcher.h"
#include "stpcomm/connection.h"
#include "codec/proto_msg_serialization.h"
#include <glog/logging.h>

StpClient& StpClient::GetInstance()
{
    static StpClient s_Instance(ConfigParser::GetInstance().svr_ip(), 
                                ConfigParser::GetInstance().svr_port());
    return s_Instance;
}

void StpClient::QuitStpClientCallback(int sig, short events, void *data)
{ 
    LOG(INFO) << "Catch the SIGINT signal, quit StpServer in one second";
    StpClient *me = reinterpret_cast<StpClient*>(data);
    timeval tv = {1, 0};
    me->StopRun(&tv);
}

void StpClient::TimerHeartBeatCallback(int sig, short events, void *data)
{
    LOG(INFO) << "process heart beat";
    StpClient *me = reinterpret_cast<StpClient*>(data);
    rpc::C2S_Ping req;
    char send_buf[2048];
    int size = static_cast<int>(sizeof(send_buf));
    if (SerializeToArray(req, send_buf, size))
    {
        me->conn_->SendNetPacket(send_buf, size);
    }
}

void StpClient::HandleConnectionEvent(Connection *conn)
{
    
}

void StpClient::HandleReadEvent(Connection *conn)
{
    if (conn == NULL)
        return;
    
    std::string pkt;
    
    if (conn->GetOneUnpackedPacket(pkt))
    {
        static StpClntMsgDispatcher s_Dispatcher;
        s_Dispatcher.OnceDispatch(conn, pkt.data(), pkt.size());
    }
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
    
    LOG(INFO) << "recv S2C_Ping";
}

StpClient::StpClient(const char *ip, int16_t port)
    : TcpEventClient(ip, port)
{
    
}

StpClient::~StpClient()
{
    
}

