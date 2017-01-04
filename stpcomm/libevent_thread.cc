#include "libevent_thread.h"
#include "tcp_event_server.h"

#include <assert.h>
#include <glog/logging.h>

#include <event2/util.h>

LibeventThread::LibeventThread(TcpEventServer *server)
    : tcp_event_server_(server)
{
    assert(server != NULL);
    //建立libevent事件处理机制    
	event_base_ = event_base_new();
	if( event_base_ == NULL)
       LOG(FATAL) << "event base new error";

	//在主线程和子线程之间建立管道
	int fds[2];
    int domain = 0;
#ifdef WIN32
    domain = AF_INET;
#else
    domain = AF_LOCAL;
#endif
	if(evutil_socketpair(domain, SOCK_STREAM, 0, fds) < 0)
        LOG(FATAL) << "errno=" << errno << ", reason is " << strerror(errno); 
    
	notify_recv_fd_ = fds[0];
	notify_send_fd_ = fds[1];

	//让子线程的状态机监听管道
	event_set( &notify_event_, notify_recv_fd_,
		EV_READ | EV_PERSIST, &LibeventThread::ThreadProcess, this );
    
	event_base_set(event_base_, &notify_event_);
	if ( event_add(&notify_event_, 0) == -1 )
		LOG(FATAL) << "Can't monitor libevent notify pipe\n";
}

LibeventThread::~LibeventThread()
{
    FreeEventBase();
}

void LibeventThread::Run()
{
	printf("thread %ud started\n", thread_id());
	event_base_dispatch(event_base_);
    printf("subthread done\n");
}

void LibeventThread::SignalExit()
{
    int contant = -1;
    write(notify_send_fd_, &contant, sizeof(int));
}

void LibeventThread::ProcessListenerEvent(evutil_socket_t fd)
{
	write(notify_send_fd_, &fd, sizeof(evutil_socket_t));
}

void LibeventThread::DeleteConnection(Connection *conn)
{
    conn_queue_.DeleteConnection(conn);
}

void LibeventThread::ThreadProcess(int fd, short which, void *arg)
{
    LibeventThread* me = reinterpret_cast<LibeventThread*>(arg);

	//从socketpair中读取数据（socket的描述符或操作码）
	evutil_socket_t confd;
	read(me->notify_recv_fd_, &confd, sizeof(evutil_socket_t));

	//如果操作码是-1，则终止事件循环
	if(confd == -1)
	{
		event_base_loopbreak(me->event_base_);
		return;
	}

	//新建连接
	struct bufferevent *bev;
	bev = bufferevent_socket_new(me->event_base_, confd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		LOG(ERROR) << "Error constructing bufferevent!";
		event_base_loopbreak(me->event_base_);
		return;
	}

	//将该链接放入队列
	Connection *conn = me->conn_queue_.InsertConnection(confd, me);

	//准备从socket中读写数据
	bufferevent_setcb(bev, &TcpEventServer::ReadEventCallback, &TcpEventServer::WriteEventCallback, 
                      &TcpEventServer::CloseEventCallback, conn);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	//调用用户自定义的连接事件处理函数
	me->tcp_event_server_->HandleConnectionEvent(conn);
}

void LibeventThread::FreeEventBase()
{
     event_base_free(event_base_);
}
