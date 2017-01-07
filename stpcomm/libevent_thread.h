#ifndef LIBEVNET_THREAD_H_
#define LIBEVNET_THREAD_H_

#include "thread.h"
#include "connection.h"
#include <event.h>

class EventNotifier;

class LibeventThread : public Thread
{
public:
    LibeventThread(EventNotifier* notifier);
    virtual ~LibeventThread();
    virtual void Run();
    
    void SignalExit();
    void ProcessListenerEvent(evutil_socket_t fd);
    void DeleteConnection(Connection* conn);
    void FreeEventBase();

    EventNotifier* GetEventNotifier();
    
private:
    
    //（主线程收到请求后），对应子线程的处理函数
	static void ThreadProcess(int fd, short which, void *arg);
    
    struct event_base* event_base_;	// libevent的事件处理机
    struct event notify_event_;	    // 监听socketpair的事件机
	int notify_recv_fd_;		    // socketpair的接收端
	int notify_send_fd_;			// socketpair的发送端
	ConnectionQueue conn_queue_;	// socket连接的链表
	// 在libevent的事件处理中要用到很多回调函数，不能使用类隐含的this指针
	// 所以用这样方式将EventNotifier的类指针传过去
	EventNotifier *event_notifier_;	// EventNotifier类的指针
};

#endif