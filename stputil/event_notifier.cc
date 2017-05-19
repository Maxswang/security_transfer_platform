#include "event_notifier.h"
#include "connection.h"

#include <event.h>
#include <glog/logging.h>

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

EventNotifier::EventNotifier(EventNotifierType ent) : 
    ent_(ent)
{
    event_base_ = event_base_new();
}

EventNotifier::~EventNotifier()
{
    event_base_free(event_base_);
}

bool EventNotifier::StartRun()
{
	// 开启主线程的事件循环
    event_base_dispatch(event_base_);
    return true;
}

bool EventNotifier::StopRun(timeval *tv)
{
	// 结果主线程的事件循环
	event_base_loopexit(event_base_, tv);
    return true;
}

void EventNotifier::ReadEventCallback(bufferevent *bev, void *data)
{
    Connection *conn = reinterpret_cast<Connection*>(data);
    conn->SetBuffer(bufferevent_get_input(bev), bufferevent_get_output(bev));
	// 调用用户自定义的读取事件处理函数
    conn->ProcessReadEvent();
}

void EventNotifier::WriteEventCallback(bufferevent *bev, void *data)
{
    Connection *conn = reinterpret_cast<Connection*>(data);
    conn->SetBuffer(bufferevent_get_input(bev), bufferevent_get_output(bev));
	// 调用用户自定义的写入事件处理函数
    conn->ProcessWriteEvent();
}

void EventNotifier::BufferEventCallback(bufferevent *bev, short events, void *data)
{
    Connection *conn = reinterpret_cast<Connection*>(data);
    
    /*events from callback*/
    if (events & EV_TIMEOUT)
    {
        LOG(INFO) << "events timeout!";
    }
    if (events & BEV_EVENT_ERROR){
        LOG(ERROR) << "buffer event error!";
    }
    if (events & (BEV_EVENT_ERROR | BEV_EVENT_EOF))
    {
        LOG(ERROR) << "Error from buffereventt!";
        bufferevent_free(bev);
        
        // 调用用户自定义的断开事件处理函数
        conn->ProcessCloseEvent(events);
        
        // 从链表中删除Connection, 并delete
        Connection::DeleteConnection(conn);
    }

    if (events &(BEV_EVENT_TIMEOUT | BEV_EVENT_READING)){
        LOG(INFO) << "read timeout!";
    }
    
    if (events & (BEV_EVENT_TIMEOUT | BEV_EVENT_WRITING)){
        LOG(ERROR) << "write timeout!";
    }
}

bool EventNotifier::AddSignalEvent(int sig, void (*ptr)(int, short, void*))
{
	// 新建一个信号事件
	event *ev = evsignal_new(event_base_, sig, ptr, this);
	if ( !ev || event_add(ev, NULL) < 0 )
	{
		event_del(ev);
		return false;
	}

	// 删除旧的信号事件（同一个信号只能有一个信号事件）
	DeleteSignalEvent(sig);
	signal_events_[sig] = ev;

	return true;
}

bool EventNotifier::DeleteSignalEvent(int sig)
{
	std::map<int, event*>::iterator iter = signal_events_.find(sig);
	if( iter == signal_events_.end() )
		return false;

	event_del(iter->second);
	signal_events_.erase(iter);
    delete iter->second;
	return true;
}

event *EventNotifier::AddTimerEvent(void (*ptr)(int, short, void *), 
								  timeval tv, bool once)
{
	short int flag = 0;
	if( !once )
		flag = EV_PERSIST;

	// 新建定时器信号事件
	event *ev = new event;
	event_assign(ev, event_base_, -1, flag, ptr, this);
	if( event_add(ev, &tv) < 0 )
	{
		event_del(ev);
		return NULL;
	}
	return ev;
}

bool EventNotifier::DeleteTimerEvent(event *ev)
{
    bool res = false;
    if (ev != NULL)
    {
        res = event_del(ev) == 0;
        delete ev;
    }
    return  res;
}