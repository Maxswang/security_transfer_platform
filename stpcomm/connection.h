#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <event2/buffer.h>

#include "event_notifier.h"

class LibeventThread;

class Connection
{
    friend class ConnectionQueue;
public:
    Connection(EventNotifier* notifier, int fd = 0) 
        : event_notifier_(notifier), fd_(fd), prev_conn_(NULL), next_conn_(NULL)
    {
        
    }
	~Connection() {}
    
    void ProcessReadEvent() 
    { event_notifier_->HandleReadEvent(this); }
    
    void ProcessWriteEvent()
    { event_notifier_->HandleWriteEvent(this); }
    
    void ProcessCloseEvent(short events)
    { event_notifier_->HandleCloseEvent(this, events); }

	int GetFd() { return fd_; }
    
    EventNotifier* event_notifier() { return event_notifier_; }
    
    void SetBuffer(evbuffer *read_buf, evbuffer *write_buf)
    {
        read_buf_ = read_buf;
        write_buf_ = write_buf;
    }

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
    
protected:
    EventNotifier* event_notifier_;
    const int fd_;				//socket的ID
	evbuffer *read_buf_;		//读数据的缓冲区
	evbuffer *write_buf_;		//写数据的缓冲区
    Connection *prev_conn_;				//前一个结点的指针
	Connection *next_conn_;				//后一个结点的指针
private:
    
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

#endif