#include "connection.h"
#include "libevent_thread.h"
#include "tcp_event_server.h"


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
	Connection *c = new Connection(t->GetTcpEventServer(), fd);
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
