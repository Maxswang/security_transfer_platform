#include "tcp_event_client.h"

#include "connection.h"
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <glog/logging.h>


TcpEventClient::TcpEventClient(const char *ip, int16_t port)
    : conn_(NULL), event_base_(NULL), buffer_event_(NULL)
{
    assert(ip != NULL);
    event_base_ = event_base_new();
    assert(event_base_ != NULL);
    
    memset(&server_addr_, 0, sizeof(server_addr_) );
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port);
    server_addr_.sin_addr.s_addr = inet_addr(ip);
}

TcpEventClient::~TcpEventClient()
{
    if (buffer_event_ != NULL)
        bufferevent_free(buffer_event_);
    if (event_base_ != NULL)
        event_base_free(event_base_);
    if (conn_ != NULL)
    {
        delete conn_;
    }
}

void TcpEventClient::StopRun(timeval *tv)
{   
    //结果主线程的事件循环
	event_base_loopexit(event_base_, tv);
}

bool TcpEventClient::StartRun()
{
    conn_ = new Connection(this);
    if (conn_ == NULL)
        return false;
    
    buffer_event_ = bufferevent_socket_new(event_base_, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(buffer_event_, &EventNotifier::ReadEventCallback, 
                      &EventNotifier::WriteEventCallback, 
                      &EventNotifier::CloseEventCallback, conn_);
    bufferevent_enable(buffer_event_, EV_READ | EV_WRITE);
    
    if (bufferevent_socket_connect(buffer_event_, 
                                   reinterpret_cast<struct sockaddr*>(&server_addr_),
                                   sizeof(server_addr_)) < 0)
    {
        bufferevent_free(buffer_event_);
        LOG(FATAL) << "Error starting connection!";
    }
    
    conn_->SetFd(bufferevent_getfd(buffer_event_));
    HandleConnectionEvent(conn_);
    
    event_base_dispatch(event_base_);
    
    return true;
}