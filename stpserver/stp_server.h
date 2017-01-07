#ifndef STP_SERVER_H_
#define STP_SERVER_H_

#include "stpcomm/tcp_event_server.h"
#include "codec/rpc.pb.h"

class StpServer : public TcpEventServer
{
public:
    static StpServer& GetInstance();
    
    static void QuitStpServerCallback(int sig, short events, void *data);
    
    // 新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn);

	// 读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn);

	// 发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn);

	// 断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events);
    
public:
    void HandleProtocol_Ping(Connection* conn, rpc::C2S_Ping* msg);
    
private:
    StpServer(int16_t port, int thread_cnt);
    virtual ~StpServer();
    
};

#endif