#include "connection.h"
#include "libevent_thread.h"
#include "event_notifier.h"

#include <assert.h>

Connection::Connection(EventNotifier *notifier, int fd) 
    : event_notifier_(notifier), fd_(fd), prev_conn_(NULL), next_conn_(NULL)
{
    
}

Connection::~Connection() 
{
    
}

void Connection::ProcessReadEvent() 
{ 
    event_notifier_->HandleReadEvent(this); 
}

void Connection::ProcessWriteEvent()
{ 
    event_notifier_->HandleWriteEvent(this); 
}

void Connection::ProcessCloseEvent(short events)
{ 
    event_notifier_->HandleCloseEvent(this, events); 
}

void Connection::DeleteConnection(Connection *c)
{
    assert(c != NULL);
    // TODO 优化
    // 判断的原因是因为TcpEventClient的prev_conn_和next_conn_都为NULL
    if (c->prev_conn_ != NULL)
    {
        c->prev_conn_->next_conn_ = c->next_conn_;
    }
    if (c->next_conn_ != NULL)
    {
        c->next_conn_->prev_conn_ = c->prev_conn_;
    }
    delete c;
}

void Connection::SetBuffer(evbuffer *read_buf, evbuffer *write_buf)
{
    assert(read_buf != NULL && write_buf != NULL);
    read_buf_ = read_buf;
    write_buf_ = write_buf;
}

size_t Connection::GetReadBufferLen()
{ 
    return evbuffer_get_length(read_buf_); 
}

int Connection::GetReadBuffer(char *buffer, int len)
{ 
    return evbuffer_remove(read_buf_, buffer, len); 
}

size_t Connection::CopyReadBuffer(char *buffer, int len)
{ 
    return evbuffer_copyout(read_buf_, buffer, len); 
}

size_t Connection::GetWriteBufferLen()
{ 
    return evbuffer_get_length(write_buf_); 
}

int Connection::AddToWriteBuffer(char *buffer, size_t len)
{ 
    return evbuffer_add(write_buf_, buffer, len); 
}

void Connection::MoveBufferData()
{ 
    evbuffer_add_buffer(write_buf_, read_buf_); 
}


ConnectionQueue::ConnectionQueue()
{
	// 建立头尾结点，并调整其指针
	head_ = new Connection(NULL);
	tail_ = new Connection(NULL);
	head_->prev_conn_ = tail_->next_conn_ = NULL;
	head_->next_conn_ = tail_;
	tail_->prev_conn_ = head_;
}

ConnectionQueue::~ConnectionQueue()
{
	Connection *cur, *next;
	cur = head_;
	// 循环删除链表中的各个结点
	while(cur != NULL)
	{
		next = cur->next_conn_;
		delete cur;
		cur = next;
	}
}

Connection *ConnectionQueue::NewConnection(EventNotifier *notifier, int fd)
{
	Connection *c = new Connection(notifier, fd);
    if (c == NULL)
        return NULL;
    
	Connection *next = head_->next_conn_;

	c->prev_conn_ = head_;
	c->next_conn_ = head_->next_conn_;
	head_->next_conn_ = c;
	next->prev_conn_ = c;
	return c;
}

void ConnectionQueue::DeleteConnection(Connection *c)
{
    assert(c != NULL);
	c->prev_conn_->next_conn_ = c->next_conn_;
	c->next_conn_->prev_conn_ = c->prev_conn_;
	delete c;
}