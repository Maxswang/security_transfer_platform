#ifndef SHM_CC_
#define SHM_CC_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*
       key_t ftok(const char *pathname, int proj_id);

       int shmget(key_t key, size_t size, int shmflg);

       void *shmat(int shmid, const void *shmaddr, int shmflg);

       int shmdt(const void *shmaddr);
*/

class SharedMemoryOper
{
public:
    SharedMemoryOper();
    ~SharedMemoryOper();
    
    int shmget(key_t key, size_t size, int shmflg);
    int shmctl(int shmid, int cmd, struct shmid_ds *buf);
    void *shmat(int shmid, const void *shmaddr, int shmflg);
    int shmdt(const void *shmaddr);
    
    void* GetShmAddr() { return shmaddr_; }
    
private:
    void* shmaddr_;
    int shmid_;
};

#endif