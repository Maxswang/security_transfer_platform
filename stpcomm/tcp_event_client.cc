#include "tcp_event_client.h"

#include "connection.h"
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <glog/logging.h>


TcpEventClient::TcpEventClient(const char *ip, int16_t port)
{
    assert(ip != NULL);
    event_base_ = event_base_new();
    assert(event_base_ != NULL);
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    
    buffer_event_ = bufferevent_socket_new(event_base_, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(buffer_event_, &TcpEventClient::ReadEventCallback, 
                      &TcpEventClient::WriteEventCallback, 
                      &TcpEventClient::CloseEventCallback, this);
    bufferevent_enable(buffer_event_, EV_READ | EV_WRITE);
    
    if (bufferevent_socket_connect(buffer_event_, 
                                   reinterpret_cast<struct sockaddr*>(&server_addr),
                                   sizeof(server_addr)) < 0)
    {
        bufferevent_free(buffer_event_);
        LOG(FATAL) << "Error starting connection!";
    }
    
    conn_ = new Connection(this, bufferevent_getfd(buffer_event_));
    if (conn_ == NULL)
    {
        bufferevent_free(buffer_event_);
        LOG(FATAL) << "new connection error!";
    }
    HandleConnectionEvent(conn_);
}

TcpEventClient::~TcpEventClient()
{
    bufferevent_free(buffer_event_);
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

void TcpEventClient::StartRun()
{
    event_base_dispatch(event_base_);
}

void TcpEventClient::ReadEventCallback(bufferevent *bev, void *data)
{
    TcpEventClient* client = reinterpret_cast<TcpEventClient*>(data);
    Connection *conn = client->conn_;
    conn->SetBuffer(bufferevent_get_input(bev), bufferevent_get_output(bev));
    conn->ProcessReadEvent();
}

void TcpEventClient::WriteEventCallback(bufferevent *bev, void *data)
{
    TcpEventClient* client = reinterpret_cast<TcpEventClient*>(data);
    Connection *conn = client->conn_;
    conn->SetBuffer(bufferevent_get_input(bev), bufferevent_get_output(bev));
    conn->ProcessWriteEvent();
}

void TcpEventClient::CloseEventCallback(bufferevent *bev, short events, void *data)
{
    TcpEventClient* client = reinterpret_cast<TcpEventClient*>(data);
    Connection *conn = client->conn_;
    conn->SetBuffer(bufferevent_get_input(bev), bufferevent_get_output(bev));
    conn->ProcessCloseEvent(events);
}
