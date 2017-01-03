#include "libevent_thread.h"
#include "tcp_event_server.h"

#include <assert.h>
#include <glog/logging.h>

LibeventThread::LibeventThread(TcpEventServer *server)
{
    assert(server != NULL);
    thread_data_.tcp_event_server = server;
    
    thread_data_.thread_id = thread_id();
    
    //建立libevent事件处理机制    
	struct event_base* base = event_base_new();
	if( base == NULL)
       LOG(FATAL) << "event base new error";
    
    thread_data_.base = base;

	//在主线程和子线程之间建立管道
	int fds[2];
	if( pipe(fds) ) // TODO
        LOG(FATAL) << "create pipe error"; 
    
	thread_data_.notify_recv_fd = fds[0];
	thread_data_.notify_send_fd = fds[1];

	//让子线程的状态机监听管道
	event_set( &thread_data_.notify_event, thread_data_.notify_recv_fd,
		EV_READ | EV_PERSIST, &LibeventThread::ThreadProcess, this );
    
	event_base_set(thread_data_.base, &thread_data_.notify_event);
	if ( event_add(&thread_data_.notify_event, 0) == -1 )
		LOG(FATAL) << "Can't monitor libevent notify pipe\n";
}

LibeventThread::~LibeventThread()
{
    FreeEventBase();
}

void LibeventThread::Run()
{
    //开启libevent的事件循环，准备处理业务
//	LibeventThread *me = reinterpret_cast<LibeventThread*>(arg);
	printf("thread %lu started\n", thread_data_.thread_id);
	event_base_dispatch(thread_data_.base);
    printf("subthread done\n");
}

void LibeventThread::SignalExit()
{
    int contant = -1;
    write(thread_data_.notify_send_fd, &contant, sizeof(int));  // TODO: writen
}

void LibeventThread::ProcessListenerEvent(evutil_socket_t fd)
{
	write(thread_data_.notify_send_fd, &fd, sizeof(evutil_socket_t)); // TODO writen
}

void LibeventThread::DeleteConnection(Connection *conn)
{
    thread_data_.conn_queue.DeleteConnection(conn);
}

void LibeventThread::ThreadProcess(int fd, short which, void *arg)
{
    LibeventThread* me = reinterpret_cast<LibeventThread*>(arg);
    LibeventThreadData& thread_data = me->thread_data_;

	//从管道中读取数据（socket的描述符或操作码）
	int pipefd = thread_data.notify_recv_fd;
	evutil_socket_t confd;
	read(pipefd, &confd, sizeof(evutil_socket_t));

	//如果操作码是EXIT_CODE，则终于事件循环
	if( -1 == confd )
	{
		event_base_loopbreak(thread_data.base);
		return;
	}

	//新建连接
	struct bufferevent *bev;
	bev = bufferevent_socket_new(thread_data.base, confd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(thread_data.base);
		return;
	}

	//将该链接放入队列
	Connection *conn = thread_data.conn_queue.InsertConnection(confd, me);

	//准备从socket中读写数据
	bufferevent_setcb(bev, &TcpEventServer::ReadEventCallback, &TcpEventServer::WriteEventCallback, &TcpEventServer::CloseEventCallback, conn);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	//调用用户自定义的连接事件处理函数
	me->thread_data_.tcp_event_server->ConnectionEvent(conn);
}

void LibeventThread::FreeEventBase()
{
     event_base_free(thread_data_.base);
}
