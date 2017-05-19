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
    
private:
    int16_t port_; // 监听的端口
};

#endif