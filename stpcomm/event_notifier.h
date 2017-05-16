#ifndef EVENT_NOTIFIER_H_
#define EVENT_NOTIFIER_H_


#include <event.h>
#include <vector>
#include <map>

enum EventNotifierType
{
    ENT_CLIENT,
    ENT_SERVER
};

class Connection;

class EventNotifier
{
public:
	// 新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn) = 0;

	// 读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn) = 0;

	// 发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn) = 0;

	// 断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events) = 0;
    
public:
    EventNotifier(EventNotifierType ent);
    virtual ~EventNotifier();
    event_base* get_event_base() { return event_base_; }
    // 开始事件循环
	bool StartRun();
    
	//在tv时间里结束事件循环, tv为NULL，则立即停止
	bool StopRun(timeval *tv);
    
    static void ReadEventCallback(struct bufferevent *bev, void *data);
	static void WriteEventCallback(struct bufferevent *bev, void *data); 
	static void BufferEventCallback(struct bufferevent *bev, short events, void *data);
    
    // 添加和删除信号处理事件, sig是信号，ptr为要回调的函数
	bool AddSignalEvent(int sig, void (*ptr)(int, short, void*));
	bool DeleteSignalEvent(int sig);

	// 添加和删除定时事件
	// ptr为要回调的函数，tv是间隔时间，once决定是否只执行一次
	event *AddTimerEvent(void(*ptr)(int, short, void*), timeval tv, bool once);
	bool DeleteTimerEvent(event *ev);
    
    
    
protected:
    event_base* event_base_;
    
	std::map<int, event*> signal_events_;	// 自定义的信号处理
    EventNotifierType ent_;
};

#endif