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

class Connection;
class TcpEventServer
{
public:
    static const int EXIT_CODE = -1;
    
	TcpEventServer(int16_t port, int thread_cnt);
	~TcpEventServer();

//设置监听的端口号，如果不需要监听，请将其设置为EXIT_CODE
//	void SetPort(uint16_t port)
//	{ m_Port = port; }

	//开始事件循环
	bool StartRun();
    
	//在tv时间里结束事件循环
	//否tv为空，则立即停止
	void StopRun(timeval *tv);

	//添加和删除信号处理事件
	//sig是信号，ptr为要回调的函数
	bool AddSignalEvent(int sig, void (*ptr)(int, short, void*));
	bool DeleteSignalEvent(int sig);

	//添加和删除定时事件
	//ptr为要回调的函数，tv是间隔时间，once决定是否只执行一次
	event *AddTimerEvent(void(*ptr)(int, short, void*), timeval tv, bool once);
	bool DeleteTimerEvent(event *ev);
    
    //被libevent回调的各个静态函数
	static void ListenerEventCallback(evconnlistener *listener, evutil_socket_t fd, 
                                      sockaddr *sa, int socklen, void *user_data);
	static void ReadEventCallback(struct bufferevent *bev, void *data);
	static void WriteEventCallback(struct bufferevent *bev, void *data); 
	static void CloseEventCallback(struct bufferevent *bev, short events, void *data);
    
private:
    int16_t port_; //监听的端口
	int thread_cnt_; //子线程数
    
    event_base* event_base_;
    std::vector<LibeventThread*> threads_;
    
	std::map<int, event*> signal_events_;	//自定义的信号处理
public:
	//这五个虚函数，一般是要被子类继承，并在其中处理具体业务的

	//新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn) { }

	//读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn) { }

	//发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn) { }

	//断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events) { }

	//发生致命错误（如果创建子线程失败等）后，会调用该函数
	//该函数的默认操作是输出错误提示，终止程序
	virtual void HandleErrorQuit(const char *str);
};

#endif