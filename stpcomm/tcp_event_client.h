#ifndef TCP_EVENT_CLIENT_H_
#define TCP_EVENT_CLIENT_H_

#include "event_notifier.h"

#include <event.h>

class TcpEventClient : public EventNotifier
{
public:
    TcpEventClient(const char* ip, int16_t port);
    virtual ~TcpEventClient();
    bool StartRun();
    
protected:
    Connection* conn_;
    struct bufferevent* buffer_event_;
    struct sockaddr_in server_addr_;
};

#endif