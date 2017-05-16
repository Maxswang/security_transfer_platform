#ifndef PROTO_MSG_SERIALIZATION_H_
#define PROTO_MSG_SERIALIZATION_H_

#include <google/protobuf/message.h>
typedef ::google::protobuf::Message ProtoMessage;

bool SerializeToArray(const ProtoMessage& msg, void* data, int& size);

#endif