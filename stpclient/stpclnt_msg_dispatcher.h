#ifndef STPCLNT_MSG_DISPATCHER_H_
#define STPCLNT_MSG_DISPATCHER_H_

#include <google/protobuf/message.h>
#include "codec/proto_msg_dispatcher.h"

class StpClntMsgDispatcher : public Net::MsgDispatcher
{
public:
    StpClntMsgDispatcher();

	DEFINE_MESSAGE(S2C_Ping);
    DEFINE_MESSAGE(S2C_StpCryptoNegotiate);
};

#endif