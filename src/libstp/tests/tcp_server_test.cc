/*
这是一个测试用的服务器，只有两个功能：
1：对于每个已连接客户端，每10秒向其发送一句hello, world
2：若客户端向服务器发送数据，服务器收到后，再将数据回发给客户端
*/
//test.cpp
#include "libstp/tcp_event_server.h"
#include <set>
#include <vector>
using namespace std;

//测试示例
class TestServer : public TcpEventServer
{
private:
	vector<Connection*> vec;
protected:
	//重载各个处理业务的虚函数
	void ReadEvent(Connection *conn);
	void WriteEvent(Connection *conn);
	void ConnectionEvent(Connection *conn);
	void CloseEvent(Connection *conn, short events);
public:
	TestServer(uint16_t port, int count) : TcpEventServer(port, count) { }
	~TestServer() { } 
	
	//退出事件，响应Ctrl+C
	static void QuitCb(int sig, short events, void *data);
	//定时器事件，每10秒向所有客户端发一句hello, world
	static void TimeOutCb(int id, int short events, void *data);
};

void TestServer::ReadEvent(Connection *conn)
{
	conn->MoveBufferData();
}

void TestServer::WriteEvent(Connection *conn)
{

}

void TestServer::ConnectionEvent(Connection *conn)
{
	TestServer *me = static_cast<TestServer*>(conn->thread()->thread_data_.tcpConnect);
	printf("new connection: %lu\n", conn->thread()->thread_data_.thread_id);
	me->vec.push_back(conn);
}

void TestServer::CloseEvent(Connection *conn, short events)
{
	printf("connection closed: %d\n", conn->GetFd());
}

void TestServer::QuitCb(int sig, short events, void *data)
{ 
	printf("Catch the SIGINT signal, quit in one second\n");
	TestServer *me = reinterpret_cast<TestServer*>(data);
    
	timeval tv = {1, 0};
	me->StopRun(&tv);
}

void TestServer::TimeOutCb(int id, short events, void *data)
{
	TestServer *me = reinterpret_cast<TestServer*>(data);
	char temp[33] = "hello, world\n";
	for(size_t i=0; i<me->vec.size(); i++)
		me->vec[i]->AddToWriteBuffer(temp, strlen(temp));
}

int main()
{
	printf("pid: %d\n", getpid());
	TestServer server(2111, 3);
	server.AddSignalEvent(SIGINT, TestServer::QuitCb);
	timeval tv = {10, 0};
	server.AddTimerEvent(TestServer::TimeOutCb, tv, false);
	server.StartRun();
	printf("done\n");
	
	return 0;
}
