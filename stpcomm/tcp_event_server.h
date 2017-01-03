#ifndef TCPEVENTSERVER_H_
#define TCPEVENTSERVER_H_

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <map>

#include <event.h>
#include <event2/listener.h>

#include "stpcomm/thread.h"

class TcpEventServer;
class Connection;
class ConnectionQueue;
struct LibeventThread;

//这个类一个链表的结点类，结点里存储各个连接的信息，
//并提供了读写数据的接口
class Connection
{
	//此类只能由TcpBaseServer创建，
	//并由ConnQueue类管理
	friend class ConnectionQueue;
	friend class TcpEventServer;

private:
	const int fd_;				//socket的ID
	evbuffer *read_buf_;		//读数据的缓冲区
	evbuffer *write_buf_;		//写数据的缓冲区

	Connection *prev_conn_;				//前一个结点的指针
	Connection *next_conn_;				//后一个结点的指针
	LibeventThread *thread_;

	Connection(int fd = 0);
	~Connection();

public:
	LibeventThread *thread() { return thread_; }
	int GetFd() { return fd_; }

	//获取可读数据的长度
	size_t GetReadBufferLen()
	{ return evbuffer_get_length(read_buf_); }

	//从读缓冲区中取出len个字节的数据，存入buffer中，若不够，则读出所有数据
	//返回读出数据的字节数
	int GetReadBuffer(char *buffer, int len)
	{ return evbuffer_remove(read_buf_, buffer, len); }

	//从读缓冲区中复制出len个字节的数据，存入buffer中，若不够，则复制出所有数据
	//返回复制出数据的字节数
	//执行该操作后，数据还会留在缓冲区中，buffer中的数据只是原数据的副本
	size_t CopyReadBuffer(char *buffer, int len)
	{ return evbuffer_copyout(read_buf_, buffer, len); }

	//获取可写数据的长度
	size_t GetWriteBufferLen()
	{ return evbuffer_get_length(write_buf_); }

	//将数据加入写缓冲区，准备发送
	int AddToWriteBuffer(char *buffer, size_t len)
	{ return evbuffer_add(write_buf_, buffer, len); }

	//将读缓冲区中的数据移动到写缓冲区
	void MoveBufferData()
	{ evbuffer_add_buffer(write_buf_, read_buf_); }

};

//带头尾结点的双链表类，每个结点存储一个连接的数据
class ConnectionQueue
{
public:
	ConnectionQueue();
	~ConnectionQueue();
    
	Connection* InsertConnection(int fd, LibeventThread* t);
	void DeleteConnection(Connection *c);
    
	//void PrintQueue();
private:
	Connection *head_;
	Connection *tail_;
};

struct LibeventThreadData
{
	unsigned long thread_id;				//线程的ID
	struct event_base* base;	//libevent的事件处理机
	struct event notifyEvent;	//监听管理的事件机
	int notifyReceiveFd;		//管理的接收端
	int notifySendFd;			//管道的发送端
	ConnectionQueue connectQueue;		//socket连接的链表
	//在libevent的事件处理中要用到很多回调函数，不能使用类隐含的this指针
	//所以用这样方式将TcpBaseServer的类指针传过去
	TcpEventServer *tcpConnect;	 //TcpBaseServer类的指针
};

class LibeventThread : public Thread
{
public:
    virtual void Run();
public:
    LibeventThreadData thread_data_;
};

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
    
private:
    int16_t port_;							//监听的端口
	int thread_cnt_;					//子线程数
//	LibeventThread *m_MainBase;			//主线程的libevent事件处理机
//	LibeventThread *m_Threads;			//存储各个子线程信息的数组
    
    LibeventThreadData main_base_;
    LibeventThread* threads_;
    
	std::map<int, event*> signal_events_;	//自定义的信号处理
    
private:
	//初始化子线程的数据
	void SetupThread(LibeventThread *thread);

	//（主线程收到请求后），对应子线程的处理函数
	static void ThreadProcess(int fd, short which, void *arg);
	//被libevent回调的各个静态函数
	static void ListenerEventCb(evconnlistener *listener, evutil_socket_t fd,
		sockaddr *sa, int socklen, void *user_data);
	static void ReadEventCallback(struct bufferevent *bev, void *data);
	static void WriteEventCallback(struct bufferevent *bev, void *data); 
	static void CloseEventCallback(struct bufferevent *bev, short events, void *data);

protected:
	//这五个虚函数，一般是要被子类继承，并在其中处理具体业务的

	//新建连接成功后，会调用该函数
	virtual void ConnectionEvent(Connection *conn) { }

	//读取完数据后，会调用该函数
	virtual void ReadEvent(Connection *conn) { }

	//发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void WriteEvent(Connection *conn) { }

	//断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void CloseEvent(Connection *conn, short events) { }

	//发生致命错误（如果创建子线程失败等）后，会调用该函数
	//该函数的默认操作是输出错误提示，终止程序
	virtual void ErrorQuit(const char *str);
};

#endif