#ifndef TCPEVENTSERVER_H_
#define TCPEVENTSERVER_H_

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <vector>
#include <map>

#include <event.h>
#include <event2/listener.h>

#include "stpcomm/thread.h"
#include "libevent_thread.h"
#include "event_notifier.h"

class Connection;
class TcpEventServer : public EventNotifier
{
public:
	TcpEventServer(int16_t port, int thread_cnt);
	~TcpEventServer();

	// 开始事件循环
	bool StartRun();
    
	//在tv时间里结束事件循环, tv为NULL，则立即停止
	void StopRun(timeval *tv);

	// 添加和删除信号处理事件, sig是信号，ptr为要回调的函数
	bool AddSignalEvent(int sig, void (*ptr)(int, short, void*));
	bool DeleteSignalEvent(int sig);

	// 添加和删除定时事件
	// ptr为要回调的函数，tv是间隔时间，once决定是否只执行一次
	event *AddTimerEvent(void(*ptr)(int, short, void*), timeval tv, bool once);
	bool DeleteTimerEvent(event *ev);
    
    // 被libevent回调的静态函数
	static void ListenerEventCallback(evconnlistener *listener, evutil_socket_t fd, 
                                      sockaddr *sa, int socklen, void *user_data);
    
private:
    int16_t port_; // 监听的端口
	int thread_cnt_; // 子线程数
    
    event_base* event_base_;
    std::vector<LibeventThread*> threads_;
    
	std::map<int, event*> signal_events_;	// 自定义的信号处理
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

};

#endif