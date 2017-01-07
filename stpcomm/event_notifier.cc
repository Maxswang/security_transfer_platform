#include "event_notifier.h"
#include "connection.h"

#include <event.h>
#include <glog/logging.h>

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

