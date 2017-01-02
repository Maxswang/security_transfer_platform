
#include "libstp/mutex.h"
#include "libstp/thread.h"
#include <stdio.h>


Mutex g_Mutex;

int g_Value = 10;

int ChangeValue()
{
    MutexGuard guard(g_Mutex);
    ++g_Value;
    return g_Value;
}

class Thread1 : public Thread
{
public:
    virtual void Run()
    {
        printf("%d\n", ChangeValue());
    }
};

int main()
{
    Thread1 t[10];
    for (int i = 0; i < 10; ++i)
    {
        t[i].Start();
    }
    
    for (int i = 0; i < 10; ++i)
    {
        t[i].Stop();
    }
    
    return 0;
}
