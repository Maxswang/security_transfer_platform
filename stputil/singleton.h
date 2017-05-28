#ifndef SINGLETON_H
#define SINGLETON_H

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

template <typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        pthread_once(&once_, &Singleton::Init);
        return *value_;
    }
    
private:
    Singleton();
    ~Singleton();
    
    static void Init()
    {
        assert(value_ == NULL);
        value_ = new T();
        ::atexit(Destroy);
    }
    
    static void Destroy()
    {
        if (value_ != NULL)
            delete value_;
    }

private:
    static T* value_;
    static pthread_once_t once_;
};

template <typename T> 
T* Singleton<T>::value_ = NULL;

template <typename T> 
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT ;



#endif // SINGLETON_H
