#ifndef PROTO_MSG_DISPATCHER_H_
#define PROTO_MSG_DISPATCHER_H_

#define DEFINE_MESSAGE(msg) void OnSync##msg(Connection* conn, google::protobuf::Message* m)

#include "rpc.pb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <map>
#include <typeinfo>
//#include <boost/python.hpp>
#include <glog/logging.h>

class Connection;

namespace Net
{
class Delegate
{
public:
    Delegate()
        : object_ptr_(0)
        , stub_ptr_(0)
    {}
    
    template <class T, void (T::*TMethod)(Connection*, google::protobuf::Message*)>
    static Delegate from_method(T* object_ptr)
    {
        Delegate d;
        d.object_ptr_ = object_ptr;
        d.stub_ptr_ = &method_stub<T, TMethod>; // #1
        return d;
    }
    
    void operator()(Connection* conn, google::protobuf::Message* a1) const
    {
        return (*stub_ptr_)(object_ptr_, conn, a1);
    }
    
private:
    typedef void (*stub_type)(void* object_ptr_, Connection*, google::protobuf::Message*);
    
    void* object_ptr_;
    stub_type stub_ptr_;
    
    template <class T, void (T::*TMethod)(Connection*, google::protobuf::Message* )>
    static void method_stub(void* object_ptr, Connection* conn, google::protobuf::Message* a1)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*TMethod)(conn, a1); // #2
    }
};
 

class MsgDispatcher
{
public:
    MsgDispatcher()
        :canceled_(false)
    {
        default_cb_ = Delegate::from_method<MsgDispatcher, &MsgDispatcher::DiscardProtobufMessage>(this);
    }
    
    virtual void DiscardProtobufMessage(Connection* conn, google::protobuf::Message* message)
    {
        LOG(INFO) << typeid(this).name() << "Discarding socket ";
        // printf("%s - Discarding client:%d, msg:%s", typeid(this).name(), lClientId, message->GetTypeName().c_str());
    }
    
//    virtual void OnPythonProc(unsigned long lClientId, const std::string & proto_name, boost::python::str & message)
//    {
//    }
    
    virtual ~MsgDispatcher()
    {
        
    }
    
    void CancelDispatch()
    {
        canceled_ = true;
    }
    
    void ResumeDispatch()
    {
        canceled_ = false;
    }
    
    bool DispatchIsCanceled() const
    {
        return canceled_;
    }
    /*
        bool LoopDispatch(const void* pData,unsigned int uSize,unsigned int& uProcessedSize, void* pContext)
        {
            unsigned int uTotalSize = uSize;
            unsigned int uProcessedOnce=0;
            uProcessedSize = 0;
            char* pBuf = static_cast<char*>( const_cast<void*>(pData));
            for(;;)
            {
                bool bRes = OnceDispatch(pBuf, uTotalSize, uProcessedOnce, pContext);
                if(bRes)
                {
                    pBuf+=uProcessedOnce;
                    uTotalSize -= uProcessedOnce;
                    uProcessedSize += uProcessedOnce;
                }
                else
                {
                    if( uSize == uProcessedSize)
                    {
                        return true;
                    }else
                    {
                        return false;
                    }
                }
            }
        }
        */
    
    google::protobuf::Message* CreateMessage(const std::string& type_name);
    google::protobuf::Message* Parse(Connection* conn, const void* pData, unsigned int uSize);
    bool OnceDispatch(Connection* conn, const void* pData, unsigned int uSize);
    
    template<typename T>
    void RegisterMessageCallback(Delegate d)
    {
        callback_map_[T::descriptor()] = d;
    }
    
private:
    bool canceled_;

    typedef std::map<const ::google::protobuf::Descriptor*, Delegate> CallbackMap;
    CallbackMap callback_map_;
    Delegate default_cb_;
};

}

#endif
