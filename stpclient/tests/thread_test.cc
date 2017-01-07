#include "stpcomm/thread.h"
#include <stdio.h>

#ifdef WIN32
DWORD WINAPI ThreadFunc(LPVOID arg)
{
	printf("child process id=%lu\n", GetCurrentThreadId());
	return 0;
}


void CreateThreadDemo()
{
	HANDLE handle = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
	WaitForSingleObject(handle, INFINITE);
}

unsigned int __stdcall ThreadFunc2(PVOID arg)
{
	printf("child process id=%lu\n", GetCurrentThreadId());
	return 0;
}

void CreateThread2Demo()
{
	const int kThreadNum = 5;
	HANDLE handle[kThreadNum] = { 0 };
	for (int i = 0; i < kThreadNum; ++i)
	{
		handle[i] = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadFunc2, NULL, 0, NULL));
	}
	WaitForMultipleObjects(kThreadNum, handle, TRUE, INFINITE);
}
#endif


class MyThread : public Thread
{
public:
	virtual void Run()
	{
		for (int i = 0; i < 10; ++i)
		{
			printf("hello %d\n", i);
			Sleep(100);
		}
	}
};

int main()
{
#ifdef WIN32
   CreateThreadDemo();
   CreateThread2Demo();
#endif

	MyThread t;
	t.Start();
	t.Stop();
    
    return 0;
}
