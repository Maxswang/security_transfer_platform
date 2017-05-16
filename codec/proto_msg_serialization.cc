#include "proto_msg_serialization.h"
#include "rpc.pb.h"

bool SerializeToArray(const ProtoMessage& msg, void *array, int& size)
{
    if (!msg.SerializeToArray(array, size))
    {
		return false;
	}
    
    rpc::Request req;
    req.set_method(msg.GetTypeName());
    req.set_serialized_request(array, msg.GetCachedSize());
    if (!req.SerializeToArray(array, size))
    {
		return false;
	}
    
	size = req.GetCachedSize();
    return true;
}
