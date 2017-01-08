#include "stpclnt_msg_dispatcher.h"

#include <glog/logging.h>
#include "stp_client.h"
#include "stpcomm/connection.h"
#include "codec/rpc.pb.h"

#define REGISTER_CALLBACK_IMP(msg) this->RegisterMessageCallback<rpc::msg>(Net::Delegate::from_method<StpClntMsgDispatcher, &StpClntMsgDispatcher::OnSync##msg>(this))
#define PROCESS_MESSAGE_START(msg)\
    void StpClntMsgDispatcher::OnSync##msg(Connection* conn, google::protobuf::Message* m)\
    {\
        rpc::msg* rst = static_cast<rpc::msg*>(m);\
		LOG(INFO) << "--->S2C : " << rst->GetTypeName().c_str();

#define PROCESS_MESSAGE_END }

StpClntMsgDispatcher::StpClntMsgDispatcher()
{
	REGISTER_CALLBACK_IMP(S2C_Ping);
}

PROCESS_MESSAGE_START(S2C_Ping)
     StpClientc::GetInstance().HandleProtocol_Ping(conn, rst);
PROCESS_MESSAGE_END