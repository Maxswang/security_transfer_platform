#ifndef TCP_EVENT_CLIENT_H_
#define TCP_EVENT_CLIENT_H_

#include "event_notifier.h"

#include <event.h>

class TcpEventClient  : public EventNotifier
{
public:
    TcpEventClient(const char* ip, int16_t port);
    virtual ~TcpEventClient();
    bool StartRun();
    void StopRun(timeval *tv);
    
    //新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn) = 0;

	//读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn) = 0;

	//发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn) = 0;

	//断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events) = 0;
    
private:
    Connection* conn_;
    struct event_base* event_base_;
    struct bufferevent* buffer_event_;
    struct sockaddr_in server_addr_;
    
};

#endif