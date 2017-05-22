#include "stp_client.h"
#include "config_parser.h"
#include "stpclnt_msg_dispatcher.h"
#include "stputil/connection.h"
#include "codec/proto_msg_serialization.h"
#include <glog/logging.h>

char StpClient::buf_[2048];

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
    int size = static_cast<int>(sizeof(buf_));
    if (SerializeToArray(req, buf_, size))
    {
        me->conn_->SendNetMessage(buf_, size);
    }
}

void StpClient::CryptoNegotiateCallback(int sig, short events, void *data)
{
    LOG(INFO) << "process crypto negotiate";
    StpClient *me = reinterpret_cast<StpClient*>(data);
    rpc::C2S_StpCryptoNegotiate req;
    req.set_stp_guid(ConfigParser::GetInstance().stp_guid());
    int size = static_cast<int>(sizeof(buf_));
    if (SerializeToArray(req, buf_, size))
    {
        me->conn_->SendNetMessage(buf_, size);
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
    
    if (conn->GetNetMessage(pkt))
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

void StpClient::HandleProtocol_CryptoNegotiate(Connection *conn, rpc::S2C_StpCryptoNegotiate *rsp)
{
    if (conn == NULL || rsp == NULL)
        return;
    
    
}

StpClient::StpClient(const char *ip, int16_t port)
    : TcpEventClient(ip, port)
{
    
}

StpClient::~StpClient()
{
    
}

