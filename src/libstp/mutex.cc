#include "mutex.h"

Mutex::Mutex()
{
#ifdef WIN32
	InitializeCriticalSection(&cs_);
#else
	pthread_mutex_init(&mutex_, NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&cs_);
#else
	pthread_mutex_destroy(&mutex_);
#endif
}

void Mutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&cs_);
#else
	pthread_mutex_lock(&mutex_);
#endif
}

void Mutex::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&cs_);
#else
	pthread_mutex_unlock(&mutex_);
#endif
}


