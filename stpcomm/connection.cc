#include "connection.h"
#include "event_notifier.h"

#include <string.h>
#include <assert.h>

Connection::Connection(EventNotifier *notifier, int fd) 
    : event_notifier_(notifier), fd_(fd)
{
    
}

Connection::~Connection() 
{
    
}

void Connection::ProcessReadEvent() 
{
    if (UnpackNetHeadPacket())
    {
        event_notifier_->HandleReadEvent(this);
    }
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
    delete c;
}

void Connection::SetBuffer(evbuffer *read_buf, evbuffer *write_buf)
{
    assert(read_buf != NULL && write_buf != NULL);
    ev_read_buf_ = read_buf;
    ev_write_buf_ = write_buf;
}

size_t Connection::GetReadBufferLen()
{ 
    return evbuffer_get_length(ev_read_buf_); 
}

int Connection::GetReadBuffer(void *buffer, size_t len)
{ 
    return evbuffer_remove(ev_read_buf_, buffer, len); 
}

size_t Connection::CopyReadBuffer(void *buffer, size_t len)
{ 
    return evbuffer_copyout(ev_read_buf_, buffer, len); 
}

size_t Connection::GetWriteBufferLen()
{ 
    return evbuffer_get_length(ev_write_buf_); 
}

int Connection::AddToWriteBuffer(const void *buffer, size_t len)
{ 
    return evbuffer_add(ev_write_buf_, buffer, len); 
}

void Connection::MoveBufferData()
{ 
    evbuffer_add_buffer(ev_write_buf_, ev_read_buf_); 
}

bool Connection::UnpackNetHeadPacket()
{
    size_t head_size = sizeof(NetHeadPacket);
    size_t read_buf_len = GetReadBufferLen();
    if (read_buf_len < head_size)
        return false;
    
    NetHeadPacket head;
    memset(&head, 0, sizeof(head));
    CopyReadBuffer(&head, head_size);
    
    head.version = ntohs(head.version);
    head.pkt_len = ntohl(head.pkt_len);
    size_t pkt_len = head.pkt_len;
    
    if (read_buf_len < pkt_len + head_size)
    {
        return false;
    }
    
    GetReadBuffer(&head, head_size);
    
    read_buf_list_.push_back(std::string(""));
    std::string& pkt_body = read_buf_list_.back();
    pkt_body.resize(pkt_len);
    
    while (pkt_len > 0) 
    {
        int read = static_cast<int>(std::min(sizeof(read_buf_), pkt_len));
        if (GetReadBuffer(read_buf_, read) == 0)
        {
            break;
        }
        pkt_len -= read;
        pkt_body.append(read_buf_, read_buf_ + read);
    }
    return true;
}

bool Connection::PackNetHeadPacket(const void *buffer, size_t len)
{
    NetHeadPacket head;
    memset(&head, 0, sizeof(head));
    head.version = htons(kNetHeadPacketVersion);
    head.pkt_len = htonl(len);
    
    AddToWriteBuffer(&head, sizeof(head));
    AddToWriteBuffer(buffer, len);
    
    return true;
}

bool Connection::GetOneUnpackedPacket(std::string& packet)
{
    if (read_buf_list_.empty())
    {
        return false;
    }
    
    packet = read_buf_list_.front();
    read_buf_list_.pop_front();
    
    return true;
}
