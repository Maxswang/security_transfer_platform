#include "thread.h"

#ifndef WIN32
#include <string.h>
#include <stdio.h>
#endif

AtomicIntegerUL Thread::num_created_;

Thread::Thread(const std::string &name) 
    : running_(false), stoped_(false), thread_name_(name)
{
    SetDefaultName();
}

Thread::~Thread() 
{
    if (!stoped_)
    {
        Stop();
    }
}

bool Thread::Start()
{
    if (running_)
        return false;
    
    running_ = true;
    stoped_ = false;
    
#ifdef WIN32
    handle_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &Thread::ThreadFunc, 
                                                      this, 0, &thread_id_));
    if (handle_ == 0)
        return false;
#else
    if (pthread_create(&pthread_id_, NULL, &Thread::ThreadFunc, this) < 0)
        return false;
#endif
    return true;
}

void Thread::Stop()
{
    stoped_ = true;
    for (int i = 0; running_ && i < 30; ++i)	//wait for 3 sec
        Thread::Sleep(100);
#ifdef WIN32
    DWORD exit_code;
    if (GetExitCodeThread(handle_, &exit_code))  
    {
        CloseHandle(handle_);  
        handle_ = NULL;
    }
#else
    pthread_join(pthread_id_, NULL);
    memset(&pthread_id_, 0, sizeof(pthread_t));
#endif
    running_ = false;
}

void Thread::Sleep(long ms)
{
#ifdef WIN32
    ::Sleep(ms);
#else
    ::usleep(static_cast<useconds_t>(ms * 1000));
#endif
}

unsigned int Thread::thread_id() 
{
#ifdef WIN32
    return thread_id_;
#else
    return static_cast<unsigned int>(pthread_id_);
#endif
}

void Thread::SetDefaultName()
{
    unsigned long num = ++num_created_;
    if (thread_name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%lu", num);
        thread_name_ = buf;
    }
}

