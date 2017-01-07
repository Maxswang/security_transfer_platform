#include "proto_msg_dispatcher.h"
//#include <boost/python.hpp>
#include <stdio.h>

namespace Net
{

google::protobuf::Message* MsgDispatcher::CreateMessage(const std::string& type_name)
{
    google::protobuf::Message* message = NULL;
    const google::protobuf::Descriptor* descriptor =
            google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor)
    {
        const google::protobuf::Message* prototype =
                google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype)
        {
            message = prototype->New();
        }
    }
    return message;
}

google::protobuf::Message* MsgDispatcher::Parse(Connection* conn, const void* pData, unsigned int uSize)
{
    rpc::Request req;
    
    if(uSize<sizeof(unsigned int))
        return NULL;
    
    if (!req.ParseFromArray(pData, uSize))
    {
        printf("Dispatcher Error:Request parse failure!\n");
        return NULL;
    }
    
    google::protobuf::Message* message = CreateMessage(req.method());
    
    if (message)
    {
        if (!message->ParseFromArray(req.serialized_request().c_str(), req.serialized_request().length()))
        {
            printf("Dispatcher Error:%s parse failure!\n", req.method().c_str());
        }
    }
    
    return message;
}

/* bool CMsgDispatcherBase::OnceDispatch(unsigned long lClientId, const void* pData, unsigned int uSize)
    {
        google::protobuf::Message* message = Parse(lClientId, pData, uSize);
        if (message)
        {
            CallbackMap::const_iterator it = m_callbacks.find(message->GetDescriptor());
            if (it != m_callbacks.end())
                it->second(lClientId, message);
            else
                m_defaultCallback(lClientId, message);
                
            delete message;
            
            return true;
        }
        
        return false;
    }*/

bool MsgDispatcher::OnceDispatch(Connection* conn, const void* pData, unsigned int uSize)
{
    rpc::Request req;
    
    if (uSize < sizeof(unsigned int))
        return false;
    
    if (!req.ParseFromArray(pData, uSize))
    {
        printf("Dispatcher Error:Request parse failure!\n");
        return false;
    }
    
    google::protobuf::Message* message = CreateMessage(req.method());
    bool processed = false;
    
    if (message)
    {
        if (!message->ParseFromArray(req.serialized_request().c_str(), req.serialized_request().length()))
        {
            printf("Dispatcher Error:%s parse failure!\n", req.method().c_str());
        }
        else
        {
            CallbackMap::const_iterator it = callback_map_.find(message->GetDescriptor());
            if (it != callback_map_.end())
            {
                it->second(conn, message);
                processed = true;
            }
        }
        
        delete message;
    }
    
    //		if (!processed)
    //		{
    //			try
    //			{
    //				std::string proto_name;
    //				std::vector<std::string> tokens;
    
    //				SplitString(req.method(), ".", tokens);
    //				if (tokens.size() == 2)
    //					proto_name = tokens.at(1);
    //				else
    //					proto_name = req.method();
    
    //				boost::python::str pyStr = boost::python::str(req.serialized_request().c_str(), req.serialized_request().length());
    //				OnPythonProc(lClientId, proto_name, pyStr);
    //			}
    //			catch (boost::python::error_already_set const &)
    //			{
    //				PyErr_Print();
    //			}
    //		}
    return true;
}

}