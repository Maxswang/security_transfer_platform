#ifndef MUTEX_H_
#define MUTEX_H_

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class Mutex 
{
public:
	Mutex();
	~Mutex();
	void Lock();
	void Unlock();
    
private:
#ifdef WIN32
	CRITICAL_SECTION cs_;
#else
	pthread_mutex_t mutex_;
#endif
};

class MutexGuard
{
public:
	MutexGuard(Mutex& mutex) : mutex_(mutex)
	{
		mutex_.Lock();
	}
	~MutexGuard()
	{
		mutex_.Unlock();
	}
private:
    Mutex& mutex_;
	MutexGuard(const MutexGuard& other);
	MutexGuard& operator=(const MutexGuard& other);
};

#endif // MUTEX_H_
