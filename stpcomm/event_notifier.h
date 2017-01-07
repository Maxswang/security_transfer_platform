#ifndef EVENT_NOTIFIER_H_
#define EVENT_NOTIFIER_H_

class Connection;

class EventNotifier
{
public:
	static void ReadEventCallback(struct bufferevent *bev, void *data);
	static void WriteEventCallback(struct bufferevent *bev, void *data); 
	static void CloseEventCallback(struct bufferevent *bev, short events, void *data);
	
	// 新建连接成功后，会调用该函数
	virtual void HandleConnectionEvent(Connection *conn) = 0;

	// 读取完数据后，会调用该函数
	virtual void HandleReadEvent(Connection *conn) = 0;

	// 发送完成功后，会调用该函数（因为串包的问题，所以并不是每次发送完数据都会被调用）
	virtual void HandleWriteEvent(Connection *conn) = 0;

	// 断开连接（客户自动断开或异常断开）后，会调用该函数
	virtual void HandleCloseEvent(Connection *conn, short events) = 0;
};

#endif