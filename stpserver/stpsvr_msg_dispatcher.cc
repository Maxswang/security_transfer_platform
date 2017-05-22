#include "stpsvr_msg_dispatcher.h"
#include <glog/logging.h>
#include "stp_server.h"
#include "stputil/connection.h"
#include "codec/rpc.pb.h"

#define REGISTER_CALLBACK_IMP(msg) this->RegisterMessageCallback<rpc::msg>(Net::Delegate::from_method<StpSvrMsgDispatcher, &StpSvrMsgDispatcher::OnSync##msg>(this))
#define PROCESS_MESSAGE_START(msg)\
    void StpSvrMsgDispatcher::OnSync##msg(Connection* conn, google::protobuf::Message* m)\
    {\
        rpc::msg* rst = static_cast<rpc::msg*>(m);\
		LOG(INFO) << "--->C2S : " << rst->GetTypeName().c_str();

#define PROCESS_MESSAGE_END }

StpSvrMsgDispatcher::StpSvrMsgDispatcher()
{
	REGISTER_CALLBACK_IMP(C2S_Ping);
    REGISTER_CALLBACK_IMP(C2S_StpCryptoNegotiate);
}

PROCESS_MESSAGE_START(C2S_Ping)
    StpServer::GetInstance().HandleProtocol_Ping(conn, rst);
PROCESS_MESSAGE_END

PROCESS_MESSAGE_START(C2S_StpCryptoNegotiate)
    StpServer::GetInstance().HandleProtocol_StpCryptoNegotiate(conn, rst);
PROCESS_MESSAGE_END