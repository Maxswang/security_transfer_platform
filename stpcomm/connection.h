#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <event2/buffer.h>

#include "event_notifier.h"

class LibeventThread;

class Connection
{
    friend class ConnectionQueue;
public:
    Connection(EventNotifier* notifier, int fd = -1);
    
	~Connection();
    
    void ProcessReadEvent();
    
    void ProcessWriteEvent();
    
    void ProcessCloseEvent(short events);
    
    static void DeleteConnection(Connection *c);

    void SetFd(int fd) { fd_ = fd; }
	int GetFd() { return fd_; }
    
    EventNotifier* event_notifier() { return event_notifier_; }
    
    void SetBuffer(evbuffer *read_buf, evbuffer *write_buf);

	// 获取可读数据的长度
	size_t GetReadBufferLen();

	// 从读缓冲区中取出len个字节的数据，存入buffer中，若不够，则读出所有数据
	// 返回读出数据的字节数
	int GetReadBuffer(char *buffer, int len);

	// 从读缓冲区中复制出len个字节的数据，存入buffer中，若不够，则复制出所有数据
	// 返回复制出数据的字节数
	// 执行该操作后，数据还会留在缓冲区中，buffer中的数据只是原数据的副本
	size_t CopyReadBuffer(char *buffer, int len);

	// 获取可写数据的长度
	size_t GetWriteBufferLen();

	// 将数据加入写缓冲区，准备发送
	int AddToWriteBuffer(char *buffer, size_t len);

	// 将读缓冲区中的数据移动到写缓冲区
	void MoveBufferData();
    
private:
    EventNotifier* event_notifier_;
    int fd_;				//socket的ID
	evbuffer *read_buf_;		//读数据的缓冲区
	evbuffer *write_buf_;		//写数据的缓冲区
    Connection *prev_conn_;				//前一个结点的指针
	Connection *next_conn_;				//后一个结点的指针
};


//带头尾结点的双链表类，每个结点存储一个连接的数据
class ConnectionQueue
{
public:
	ConnectionQueue();
	~ConnectionQueue();
    
	Connection* NewConnection(EventNotifier* notifier, int fd);
	void DeleteConnection(Connection *c);
    
	//void PrintQueue();
private:
	Connection *head_;
	Connection *tail_;
};

#endif