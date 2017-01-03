#ifndef THREAD_H_
#define THREAD_H_

#include "atomic.h"
#include <string>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

class Thread
{
public:
	Thread(const std::string& name = "");
	virtual ~Thread();

	bool Start();

	void Stop();

	static void Sleep(long ms);
    
    static unsigned long CreatedThreadNum() { return num_created_.value(); }
    
	virtual void Run() {}

	bool running() { return running_; }
	bool stoped() { return stoped_; }
    unsigned int thread_id();

protected:

#ifdef WIN32
	unsigned int thread_id_;
	HANDLE handle_;
#else
	pthread_t pthread_id_;
#endif
	bool running_;
	bool stoped_;
	std::string thread_name_;
	static AtomicIntegerUL num_created_;

private:

#ifdef WIN32
	static unsigned int __stdcall ThreadFunc(PVOID arg) {
#else
	static void* ThreadFunc(void* arg) {
#endif
		Thread *thread = reinterpret_cast<Thread *>(arg);
		thread->running_ = true;
		thread->Run();
		thread->running_ = false;
		return 0;
	}
    
    void SetDefaultName();
};

#endif // THREAD_H_