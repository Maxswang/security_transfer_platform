#include "tcp_event_server.h"
#include "stpcomm/connection.h"
#include <glog/logging.h>
#include <assert.h>

TcpEventServer::TcpEventServer(int16_t port)
    : EventNotifier(ENT_SERVER), port_(port)
{
	
}

TcpEventServer::~TcpEventServer()
{
	//停止事件循环（如果事件循环没开始，则没效果）
	StopRun(NULL);
}

bool TcpEventServer::StartRun()
{
	evconnlistener *listener = NULL;

	// 如果端口号不是-1，就监听该端口号
	if( port_ != -1 )
	{
		sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port_);
		listener = evconnlistener_new_bind(get_event_base(), 
                                           ListenerEventCallback, 
                                           this, 
                                           LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 
                                           -1, 
                                           reinterpret_cast<sockaddr*>(&sin), 
                                           sizeof(sockaddr_in));
		if( NULL == listener )
			LOG(FATAL) << "TCP listen error!";
	}

    EventNotifier::StartRun();

	// 事件循环结果，释放监听者的内存
	if(port_ != -1)
	{
		LOG(INFO) << "free listen";
		evconnlistener_free(listener);
	}
    return true;
}

void TcpEventServer::ListenerEventCallback(struct evconnlistener *listener, 
									evutil_socket_t fd,
									struct sockaddr *sa, 
									int socklen, 
									void *user_data)
{
	TcpEventServer *server = reinterpret_cast<TcpEventServer*>(user_data);

    // 如果操作码是-1，则终止事件循环
    assert(fd != -1);
    
    // 新建连接
    struct bufferevent *bev;
    bev = bufferevent_socket_new(server->get_event_base(), fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        LOG(ERROR) << "Error constructing bufferevent!";
        event_base_loopbreak(server->get_event_base());
        return;
    }
    
    // 将该链接放入队列
    Connection *conn = new Connection(server, fd);
    
    // 准备从socket中读写数据
    bufferevent_setcb(bev, &EventNotifier::ReadEventCallback, 
                      &EventNotifier::WriteEventCallback, 
                      &EventNotifier::BufferEventCallback, conn);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);
    
    // 调用用户自定义的连接事件处理函数
    server->HandleConnectionEvent(conn);
}


