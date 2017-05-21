#ifndef STP_CRYPTO_NEGOTIATE_H_
#define STP_CRYPTO_NEGOTIATE_H_

#include "stp_idx_mgr.h"
#include "stputil/shm.h"
#include "stpserver/config_parser.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <glog/logging.h>


#include <map>

struct CryptoItem
{
    char key[36]; // 只用其中32-35位
    char zero[28]; // 扩展用
}__attribute__((packed));

struct CryptoGroup
{
public:
    CryptoGroup(int g);
    ~CryptoGroup() {}
    
    CryptoItem* GetCryptoItemByIdx(int idx);
    void* TryAttachShm(size_t size);
    bool CreateShm(size_t size); 

    int group;
    SharedMemoryOper shm_oper;
    int shmid;
    void* shm_addr;
    StpIdxMgr& idx_mgr;
};

class StpCryptoNegotiate
{
public:
    static StpCryptoNegotiate& GetInstance();
    
    bool Init();
    
    CryptoItem* GetCryptoItem(int group, int idx);
    
private:
    StpCryptoNegotiate();
    ~StpCryptoNegotiate();
    
    StpCryptoNegotiate(const StpCryptoNegotiate&);
    StpCryptoNegotiate& operator=(const StpCryptoNegotiate&);
    
private:
    std::map<int, CryptoGroup> shm_map_; // <组id, 共享内存地址> 
    int cur_group_; // 当前创建的是第几组
    bool initialized_;
};

#endif