
//tcp_client_test.cc
#include "stpcomm/tcp_event_client.h"
#include "stpcomm/connection.h"
#include <glog/logging.h>

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <set>
#include <vector>
using namespace std;

//测试示例
class TestClient : public TcpEventClient
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
	TestClient(const char* ip, int16_t port) : TcpEventClient(ip, port) 
    { }
	~TestClient() { } 
	
	//退出事件，响应Ctrl+C
	static void QuitCb(int sig, short events, void *data);
	//定时器事件，每10秒向所有客户端发一句hello, world
	static void TimeOutCb(int id, int short events, void *data);
};

void TestClient::HandleReadEvent(Connection *conn)
{
	size_t size = conn->GetReadBufferLen();
    char buf[512] = {0};
    int read = 0;
    LOG(INFO) << "recv data" ;
    while (size > 0) 
    {
        read = static_cast<int>(std::min(sizeof(buf), size));
        if (conn->GetReadBuffer(buf, read) == 0)
        {
            break;
        }
        size -= read;
        for (int i = 0; i < read; ++i)
        {
            printf("%c", buf[i]);
        }
        printf("\n");
    }
}

void TestClient::HandleWriteEvent(Connection *conn)
{
    
}

void TestClient::HandleConnectionEvent(Connection *conn)
{
	TestClient *me = static_cast<TestClient*>(conn->event_notifier());
	printf("new connection: %d\n", conn->GetFd());
	me->vec.push_back(conn);
}

void TestClient::HandleCloseEvent(Connection *conn, short events)
{
	printf("connection closed: %d\n", conn->GetFd());
}

void TestClient::QuitCb(int sig, short events, void *data)
{ 
	printf("Catch the SIGINT signal, quit in one second\n");
	TestClient *me = reinterpret_cast<TestClient*>(data);
    
	timeval tv = {1, 0};
	me->StopRun(&tv);
}

void TestClient::TimeOutCb(int id, short events, void *data)
{
	TestClient *me = reinterpret_cast<TestClient*>(data);
	char temp[33] = "hello, world\n";
	for(size_t i=0; i<me->vec.size(); i++)
		me->vec[i]->AddToWriteBuffer(temp, strlen(temp));
}

int main()
{
	printf("pid: %d\n", getpid());
	TestClient client("127.0.0.1", 2111);
//	client.AddSignalEvent(SIGINT, TestClient::QuitCb);
//	timeval tv = {10, 0};
//	server.AddTimerEvent(TestClient::TimeOutCb, tv, false);
	client.StartRun();
	printf("done\n");
	
	return 0;
}
