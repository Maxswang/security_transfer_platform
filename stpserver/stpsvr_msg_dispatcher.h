#ifndef STPSVR_MSG_DISPATCHER_H_
#define STPSVR_MSG_DISPATCHER_H_

#include <google/protobuf/message.h>
#include "codec/proto_msg_dispatcher.h"

class StpSvrMsgDispatcher : public Net::MsgDispatcher
{
public:
    StpSvrMsgDispatcher();

	DEFINE_MESSAGE(C2S_Ping);
};


#endif