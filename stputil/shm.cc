#include "shm.h"
#include <stdlib.h>

SharedMemoryOper::SharedMemoryOper()
    : shmaddr_(NULL), shmid_(-1)
{
    
}

SharedMemoryOper::~SharedMemoryOper()
{
    
}

int SharedMemoryOper::shmget(key_t key, size_t size, int shmflg)
{
    shmid_ = ::shmget(key, size, shmflg);
    return shmid_;
}

int SharedMemoryOper::shmctl(int shmid, int cmd, shmid_ds *buf)
{
    return ::shmctl(shmid, cmd, buf);
}

void *SharedMemoryOper::shmat(int shmid, const void *shmaddr, int shmflg)
{
    shmaddr_ = ::shmat(shmid, shmaddr, shmflg);
    return shmaddr_;
}

int SharedMemoryOper::shmdt(const void *shmaddr)
{
    return shmdt(shmaddr);
}
