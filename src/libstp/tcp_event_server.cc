#include "tcp_event_server.h"

Connection::Connection(int fd) : fd_(fd)
{
	prev_conn_ = NULL;
	next_conn_ = NULL;
}

Connection::~Connection()
{

}

ConnectionQueue::ConnectionQueue()
{
	//建立头尾结点，并调整其指针
	head_ = new Connection(0);
	tail_ = new Connection(0);
	head_->prev_conn_ = tail_->next_conn_ = NULL;
	head_->next_conn_ = tail_;
	tail_->prev_conn_ = head_;
}

ConnectionQueue::~ConnectionQueue()
{
	Connection *tcur, *tnext;
	tcur = head_;
	//循环删除链表中的各个结点
	while( tcur != NULL )
	{
		tnext = tcur->next_conn_;
		delete tcur;
		tcur = tnext;
	}
}

Connection *ConnectionQueue::InsertConnection(int fd, LibeventThread *t)
{
	Connection *c = new Connection(fd);
	c->thread_ = t;
	Connection *next = head_->next_conn_;

	c->prev_conn_ = head_;
	c->next_conn_ = head_->next_conn_;
	head_->next_conn_ = c;
	next->prev_conn_ = c;
	return c;
}

void ConnectionQueue::DeleteConnection(Connection *c)
{
	c->prev_conn_->next_conn_ = c->next_conn_;
	c->next_conn_->prev_conn_ = c->prev_conn_;
	delete c;
}

/*
void ConnQueue::PrintQueue()
{
	Conn *cur = m_head->m_Next;
	while( cur->m_Next != NULL )
	{
		printf("%d ", cur->m_fd);
		cur = cur->m_Next;
	}
	printf("\n");
}
*/

TcpEventServer::TcpEventServer(int16_t port, int thread_cnt)
    : port_(port), thread_cnt_(thread_cnt)
{
	//初始化各项数据
//	m_MainBase = new LibeventThread;
	threads_ = new LibeventThread[thread_cnt_];
	main_base_.thread_id = pthread_self();
	main_base_.base = event_base_new();

	//初始化各个子线程的结构体
	for(int i=0; i<thread_cnt_; i++)
	{
		SetupThread(&threads_[i]);
	}

}

TcpEventServer::~TcpEventServer()
{
	//停止事件循环（如果事件循环没开始，则没效果）
	StopRun(NULL);

	//释放内存
	event_base_free(main_base_.base);
	for(int i=0; i<thread_cnt_; i++)
		event_base_free(threads_[i].thread_data_.base);

	delete [] threads_;
}

void TcpEventServer::ErrorQuit(const char *str)
{
	//输出错误信息，退出程序
	fprintf(stderr, "%s", str);   
	if( errno != 0 )    
		fprintf(stderr, " : %s", strerror(errno));    
	fprintf(stderr, "\n");        
	exit(1);    
}

void TcpEventServer::SetupThread(LibeventThread *me)
{
	//建立libevent事件处理机制    
    LibeventThreadData& thread_data = me->thread_data_;
    
    thread_data.tcpConnect = this;
	thread_data.base = event_base_new();
	if( NULL == thread_data.base )
		ErrorQuit("event base new error");

	//在主线程和子线程之间建立管道
	int fds[2];
	if( pipe(fds) )
		ErrorQuit("create pipe error");
	thread_data.notifyReceiveFd = fds[0];
	thread_data.notifySendFd = fds[1];

	//让子线程的状态机监听管道
	event_set( &thread_data.notifyEvent, thread_data.notifyReceiveFd,
		EV_READ | EV_PERSIST, ThreadProcess, me );
    
	event_base_set(thread_data.base, &thread_data.notifyEvent);
	if ( event_add(&thread_data.notifyEvent, 0) == -1 )
		ErrorQuit("Can't monitor libevent notify pipe\n");
}

bool TcpEventServer::StartRun()
{
	evconnlistener *listener;

	//如果端口号不是EXIT_CODE，就监听该端口号
	if( port_ != EXIT_CODE )
	{
		sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port_);
		listener = evconnlistener_new_bind(main_base_.base, 
			ListenerEventCb, this,
			LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
			reinterpret_cast<sockaddr*>(&sin), sizeof(sockaddr_in));
		if( NULL == listener )
			ErrorQuit("TCP listen error");
	}

	//开启各个子线程
	for(int i=0; i<thread_cnt_; i++)
	{
//		pthread_create(&m_Threads[i].tid, NULL,  
//			WorkerLibevent, reinterpret_cast<void*>(&m_Threads[i]));
        threads_[i].Start();
	}

	//开启主线程的事件循环
	event_base_dispatch(main_base_.base);

	//事件循环结果，释放监听者的内存
	if( port_ != EXIT_CODE )
	{
		printf("free listen\n");
		evconnlistener_free(listener);
	}
    return true;
}

void TcpEventServer::StopRun(timeval *tv)
{
	int contant = EXIT_CODE;
	//向各个子线程的管理中写入EXIT_CODE，通知它们退出
	for(int i=0; i<thread_cnt_; i++)
	{
		write(threads_[i].thread_data_.notifySendFd, &contant, sizeof(int));
	}
	//结果主线程的事件循环
	event_base_loopexit(main_base_.base, tv);
}

void TcpEventServer::ListenerEventCb(struct evconnlistener *listener, 
									evutil_socket_t fd,
									struct sockaddr *sa, 
									int socklen, 
									void *user_data)
{
	TcpEventServer *server = reinterpret_cast<TcpEventServer*>(user_data);

	//随机选择一个子线程，通过管道向其传递socket描述符
	int num = rand() % server->thread_cnt_;
	int sendfd = server->threads_[num].thread_data_.notifySendFd;
	write(sendfd, &fd, sizeof(evutil_socket_t));
}

void TcpEventServer::ThreadProcess(int fd, short which, void *arg)
{
	LibeventThread *me = reinterpret_cast<LibeventThread*>(arg);
    LibeventThreadData& thread_data = me->thread_data_;

	//从管道中读取数据（socket的描述符或操作码）
	int pipefd = thread_data.notifyReceiveFd;
	evutil_socket_t confd;
	read(pipefd, &confd, sizeof(evutil_socket_t));

	//如果操作码是EXIT_CODE，则终于事件循环
	if( EXIT_CODE == confd )
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
	Connection *conn = thread_data.connectQueue.InsertConnection(confd, me);

	//准备从socket中读写数据
	bufferevent_setcb(bev, ReadEventCallback, WriteEventCallback, CloseEventCallback, conn);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	//调用用户自定义的连接事件处理函数
	me->thread_data_.tcpConnect->ConnectionEvent(conn);
}

void TcpEventServer::ReadEventCallback(struct bufferevent *bev, void *data)
{
	Connection *conn = reinterpret_cast<Connection*>(data);
    
	conn->read_buf_ = bufferevent_get_input(bev);
	conn->write_buf_ = bufferevent_get_output(bev);

	//调用用户自定义的读取事件处理函数
	conn->thread_->thread_data_.tcpConnect->ReadEvent(conn);
} 

void TcpEventServer::WriteEventCallback(struct bufferevent *bev, void *data)
{
	Connection *conn = reinterpret_cast<Connection*>(data);
	conn->read_buf_ = bufferevent_get_input(bev);
	conn->write_buf_ = bufferevent_get_output(bev);

	//调用用户自定义的写入事件处理函数
	conn->thread_->thread_data_.tcpConnect->WriteEvent(conn);

}

void TcpEventServer::CloseEventCallback(struct bufferevent *bev, short events, void *data)
{
	Connection *conn = reinterpret_cast<Connection*>(data);
	//调用用户自定义的断开事件处理函数
	conn->thread_->thread_data_.tcpConnect->CloseEvent(conn, events);
	conn->thread()->thread_data_.connectQueue.DeleteConnection(conn);
	bufferevent_free(bev);
}

bool TcpEventServer::AddSignalEvent(int sig, void (*ptr)(int, short, void*))
{
	//新建一个信号事件
	event *ev = evsignal_new(main_base_.base, sig, ptr, this);
	if ( !ev || event_add(ev, NULL) < 0 )
	{
		event_del(ev);
		return false;
	}

	//删除旧的信号事件（同一个信号只能有一个信号事件）
	DeleteSignalEvent(sig);
	signal_events_[sig] = ev;

	return true;
}

bool TcpEventServer::DeleteSignalEvent(int sig)
{
	std::map<int, event*>::iterator iter = signal_events_.find(sig);
	if( iter == signal_events_.end() )
		return false;

	event_del(iter->second);
	signal_events_.erase(iter);
	return true;
}

event *TcpEventServer::AddTimerEvent(void (*ptr)(int, short, void *), 
								  timeval tv, bool once)
{
	short int flag = 0;
	if( !once )
		flag = EV_PERSIST;

	//新建定时器信号事件
	event *ev = new event;
	event_assign(ev, main_base_.base, -1, flag, ptr, this);
	if( event_add(ev, &tv) < 0 )
	{
		event_del(ev);
		return NULL;
	}
	return ev;
}

bool TcpEventServer::DeleteTimerEvent(event *ev)
{
    return  event_del(ev) == 0;
}
void LibeventThread::Run()
{
    //开启libevent的事件循环，准备处理业务
//	LibeventThread *me = reinterpret_cast<LibeventThread*>(arg);
	printf("thread %lu started\n", thread_data_.thread_id);
	event_base_dispatch(thread_data_.base);
	printf("subthread done\n");
}
