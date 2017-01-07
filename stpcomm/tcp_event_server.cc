#include "tcp_event_server.h"

#include <glog/logging.h>
#include <assert.h>

TcpEventServer::TcpEventServer(int16_t port, int thread_cnt)
    : port_(port), thread_cnt_(thread_cnt)
{
	event_base_ = event_base_new();

	// 初始化各个子线程的结构体
	for(int i=0; i < thread_cnt_; i++)
	{
		threads_.push_back(new LibeventThread(this));
	}
}

TcpEventServer::~TcpEventServer()
{
	//停止事件循环（如果事件循环没开始，则没效果）
	StopRun(NULL);
	event_base_free(event_base_);
	for(int i=0; i < thread_cnt_; ++i)
        delete threads_[i];
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
		listener = evconnlistener_new_bind(event_base_, 
                                           ListenerEventCallback, 
                                           this, 
                                           LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 
                                           -1, 
                                           reinterpret_cast<sockaddr*>(&sin), 
                                           sizeof(sockaddr_in));
		if( NULL == listener )
			LOG(FATAL) << "TCP listen error!";
	}

	// 开启各个子线程
	for(int i=0; i<thread_cnt_; i++)
	{
        threads_[i]->Start();
	}

	// 开启主线程的事件循环
	event_base_dispatch(event_base_);

	// 事件循环结果，释放监听者的内存
	if(port_ != -1)
	{
		LOG(INFO) << "free listen";
		evconnlistener_free(listener);
	}
    return true;
}

void TcpEventServer::StopRun(timeval *tv)
{
	// 向各个子线程的socketpair中写入-1，通知它们退出
	for(int i=0; i<thread_cnt_; i++)
	{
        threads_[i]->SignalExit();
	}
	// 结果主线程的事件循环
	event_base_loopexit(event_base_, tv);
}

void TcpEventServer::ListenerEventCallback(struct evconnlistener *listener, 
									evutil_socket_t fd,
									struct sockaddr *sa, 
									int socklen, 
									void *user_data)
{
	TcpEventServer *server = reinterpret_cast<TcpEventServer*>(user_data);
	// 随机选择一个子线程，通过socketpair向其传递socket描述符
	int num = rand() % server->thread_cnt_;
    server->threads_[num]->ProcessListenerEvent(fd);

}

bool TcpEventServer::AddSignalEvent(int sig, void (*ptr)(int, short, void*))
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

bool TcpEventServer::DeleteTimerEvent(event *ev)
{
    return  event_del(ev) == 0;
}
