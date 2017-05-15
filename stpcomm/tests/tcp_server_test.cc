/*
这是一个测试用的服务器，只有两个功能：
1：对于每个已连接客户端，每10秒向其发送一句hello, world
2：若客户端向服务器发送数据，服务器收到后，再将数据回发给客户端
*/
//tcp_server_test.cc
#include "stpcomm/tcp_event_server.h"
#include <set>
#include <vector>
#include <glog/logging.h>
#include "stpcomm/connection.h"
using namespace std;

#include <errno.h>
#include <signal.h>

//测试示例
class TestServer : public TcpEventServer
{
private:
	vector<Connection*> vec;
protected:
	//重载各个处理业务的虚函数
	void HandleReadEvent(Connection *conn);
	void HandleWriteEvent(Connection *conn);
	void HandleConnectionEvent(Connection *conn);
	void HandleCloseEvent(Connection *conn, short events);
public:
	TestServer(uint16_t port) : TcpEventServer(port) { }
	~TestServer() { } 
	
	//退出事件，响应Ctrl+C
	static void QuitCb(int sig, short events, void *data);
	//定时器事件，每10秒向所有客户端发一句hello, world
	static void TimeOutCb(int id, int short events, void *data);
};

void TestServer::HandleReadEvent(Connection *conn)
{
    LOG(INFO) << "fd: " << conn->GetFd() << ", datalen=" << conn->GetReadBufferLen();
	conn->MoveBufferData();
}

void TestServer::HandleWriteEvent(Connection *conn)
{
    LOG(INFO) << "HandleWriteEvent ";
}

void TestServer::HandleConnectionEvent(Connection *conn)
{
    LOG(INFO) << "new connection fd: " << conn->GetFd();
	vec.push_back(conn);
}

void TestServer::HandleCloseEvent(Connection *conn, short events)
{
    LOG(INFO) << "connection closed fd: " << conn->GetFd();
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i]->GetFd() == conn->GetFd())
        {
            vec.erase(vec.begin() + i);
            break;
        }
    }
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
	{
        Connection* conn = me->vec[i];
        bool ret = conn->PackNetHeadPacket(temp, strlen(temp));
//        int ret = me->vec[i]->AddToWriteBuffer(temp, strlen(temp));
        LOG(INFO) << "AddToWriteBuffer ret: " << ret;
    }
		
}

int main()
{
    google::InstallFailureSignalHandler();
	printf("pid: %d\n", getpid());
	TestServer server(2111);
	server.AddSignalEvent(SIGINT, TestServer::QuitCb);
	timeval tv = {2, 0};
	server.AddTimerEvent(TestServer::TimeOutCb, tv, false);
	server.StartRun();
	printf("done\n");
	
	return 0;
}
