#ifndef TCPEVENTSERVER_H_
#define TCPEVENTSERVER_H_

#include <stdlib.h>
#include <string.h>

#include <event.h>
#include <event2/listener.h>
#include "event_notifier.h"

class Connection;
class TcpEventServer : public EventNotifier
{
public:
	TcpEventServer(int16_t port);
	virtual ~TcpEventServer();
    bool StartRun();
    
    // 被libevent回调的静态函数
	static void ListenerEventCallback(evconnlistener *listener, evutil_socket_t fd, 
                                      sockaddr *sa, int socklen, void *user_data);
    
public:
	// 这四个虚函数，一般是要被子类继承，并在其中处理具体业务的

	// 新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn) = 0;

	// 读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn) = 0;

	// 发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn) = 0;

	// 断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events) = 0;
    
private:
    int16_t port_; // 监听的端口
};

#endif