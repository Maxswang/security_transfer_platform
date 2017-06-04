#ifndef STP_CRYPTO_DETAIL_H_
#define STP_CRYPTO_DETAIL_H_

#include "stputil/shm.h"
#include "stp_idx_mgr.h"

struct CryptoItem
{
    int group;       // 组下标
    int idx;         // 索引的下标
    char key[36];    // 只用其中32-35位
    bool use_crypto; // 是否使用协商的密钥
    int64_t expires; // 过期时间
    char zero[11];   // 扩展用
    
    bool GenerateRandomKey();
}__attribute__((packed));

struct CryptoGroup
{
public:
    CryptoGroup(int g = -1, int max_idx = -1, key_t key = -1);
    ~CryptoGroup() {}
    
    CryptoItem* GetCryptoItemByIdx(int idx);
    void* TryAttachShm(size_t size);
    bool CreateShm(size_t size); 

    key_t key_;
    int group;
    SharedMemoryOper shm_oper;
    int shmid;
    void* shm_addr;
    StpIdxMgr idx_mgr;
};

#endif