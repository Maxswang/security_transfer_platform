#include "stp_client.h"
#include "config_parser.h"
#include "stpclnt_msg_dispatcher.h"
#include "stputil/connection.h"
#include "codec/proto_msg_serialization.h"
#include <glog/logging.h>

#include <string.h>

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

bool StpClient::Init()
{
    void* shm_addr = cg_.TryAttachShm(sizeof(CryptoItem));
    if (shm_addr != NULL)
    {
        cg_.shm_addr = shm_addr;
        LOG(INFO) << "try attach shm succeed!";
        return true;
    }
    else
    {
        if (!cg_.CreateShm(sizeof(CryptoItem)))
        {
            LOG(ERROR) << "create a empty crypto group failed!" ;
            return false;
        }
        LOG(INFO) << "create a empty crypto group succeed!" ;
        return true;
    }
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
    
    if (rsp->res() != rpc::SR_OK)
    {
        LOG(ERROR) << "CryptoNegotiate failed, res is " << rsp->res();
        // 更新共享内存密钥信息
        UpdateCryptoShmInfo(rsp->res(), rsp->token());
        return;
    }
    // 更新共享内存密钥信息
    UpdateCryptoShmInfo(rsp->res(), rsp->token());
    
    LOG(INFO) << "CryptoNegotiate token is " << rsp->token().DebugString();
}

bool StpClient::UpdateCryptoShmInfo(rpc::StpResult res, const rpc::StpToken &token)
{
    bool ret = false;
    
    CryptoItem* item = cg_.GetCryptoItemByIdx(0);
    if (item != NULL)
    {
        item->group = token.group();
        item->idx = token.idx();
        
        if (res != rpc::SR_OK)
        {
            memset(item->key, 0x00, sizeof(item->key));
            item->use_crypto = false;
            item->expires = -1;
            ret = true;
        }
        else
        {
            memset(item->key, 0x00, sizeof(item->key));
            strcpy(item->key, token.key().c_str());
            item->use_crypto = true;
            item->expires = token.expires();
            ret = true;
        }
        LOG(INFO) << "update stpclient crypto succeed!";
    }
    else
    {
        LOG(ERROR) << "CryptoItem is NULL";
    }
    return ret;
}

StpClient::StpClient(const char *ip, int16_t port)
    : TcpEventClient(ip, port), 
      cg_(0, 
          ConfigParser::GetInstance().max_idx(),
          ftok(ConfigParser::GetInstance().path().c_str(), 0))
{
    
}

StpClient::~StpClient()
{
    
}

