#include "tcp_event_client.h"

#include "connection.h"
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <glog/logging.h>


TcpEventClient::TcpEventClient(const char *ip, int16_t port)
    : EventNotifier(ENT_CLIENT), conn_(NULL), buffer_event_(NULL)
{
    assert(ip != NULL);
    
    memset(&server_addr_, 0, sizeof(server_addr_) );
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port);
    server_addr_.sin_addr.s_addr = inet_addr(ip);
}

TcpEventClient::~TcpEventClient()
{
    if (buffer_event_ != NULL)
        bufferevent_free(buffer_event_);
    
    if (conn_ != NULL)
    {
        delete conn_;
    }
}


bool TcpEventClient::StartRun()
{
    conn_ = new Connection(this);
    if (conn_ == NULL)
        return false;
    
    buffer_event_ = bufferevent_socket_new(get_event_base(), -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(buffer_event_, &EventNotifier::ReadEventCallback, 
                      &EventNotifier::WriteEventCallback, 
                      &EventNotifier::BufferEventCallback, conn_);
    bufferevent_enable(buffer_event_, EV_READ | EV_WRITE);
    
    if (bufferevent_socket_connect(buffer_event_, 
                                   reinterpret_cast<struct sockaddr*>(&server_addr_),
                                   sizeof(server_addr_)) < 0)
    {
        bufferevent_free(buffer_event_);
        LOG(FATAL) << "Error starting connection!";
    }
    
    conn_->SetFd(bufferevent_getfd(buffer_event_));
    conn_->SetBuffer(bufferevent_get_input(buffer_event_), bufferevent_get_output(buffer_event_));
    HandleConnectionEvent(conn_);
    
    EventNotifier::StartRun();
    
    return true;
}