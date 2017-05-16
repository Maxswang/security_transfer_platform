#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <event2/buffer.h>

#include "event_notifier.h"

#include <list>
#include <string>

class LibeventThread;

const uint8_t kNetHeadPacketVersion = 10;

#pragma pack(2) 
struct NetHeadPacket
{
    uint8_t version;
    uint32_t pkt_len;
};
#pragma pack()

class Connection
{
    friend class ConnectionQueue;
public:
    typedef std::list<std::string> BufferList;
    typedef BufferList::iterator BufferListIterator;
    
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
	int GetReadBuffer(void *buffer, size_t len);

	// 从读缓冲区中复制出len个字节的数据，存入buffer中，若不够，则复制出所有数据
	// 返回复制出数据的字节数
	// 执行该操作后，数据还会留在缓冲区中，buffer中的数据只是原数据的副本
	size_t CopyReadBuffer(void *buffer, size_t len);

	// 获取可写数据的长度
	size_t GetWriteBufferLen();

	// 将数据加入写缓冲区，准备发送
	int AddToWriteBuffer(const void *buffer, size_t len);

	// 将读缓冲区中的数据移动到写缓冲区
	void MoveBufferData();
    
    bool UnpackNetHeadPacket();
    bool PackNetHeadPacket(const void* buffer, size_t len);
    bool GetOneUnpackedPacket(std::string &packet);
    void DelteUnpacketPacket(BufferListIterator& it);
    bool SendNetPacket(const void* buffer, size_t len);
    
private:
    EventNotifier* event_notifier_;
    int fd_;				//socket的ID
	evbuffer *ev_read_buf_;		//读数据的缓冲区
	evbuffer *ev_write_buf_;		//写数据的缓冲区
    
    
    BufferList read_buf_list_;
    BufferList write_buf_list_;
    char read_buf_[1024];
//    char write_buf_[1024];
};


#endif