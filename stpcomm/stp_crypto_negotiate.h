#ifndef STP_CRYPTO_NEGOTIATE_H_
#define STP_CRYPTO_NEGOTIATE_H_

#include "stp_idx_mgr.h"
#include "stputil/shm.h"
#include "stpserver/config_parser.h"
#include "stp_crypto_detail.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <glog/logging.h>


#include <map>



class StpCryptoNegotiate
{
public:
    static StpCryptoNegotiate& GetInstance();
    
    bool Init();
    
    CryptoItem* GetCryptoItem(int group, int idx);
    bool CryptoNegotiate(int& group, int& idx, std::string& key);
    
private:
    StpCryptoNegotiate();
    ~StpCryptoNegotiate();
    
    StpCryptoNegotiate(const StpCryptoNegotiate&);
    StpCryptoNegotiate& operator=(const StpCryptoNegotiate&);
    
private:
    std::map<int, CryptoGroup> shm_map_; // <组id, 共享内存地址> 
    typedef std::map<int, CryptoGroup>::iterator ShmMapIter;
    int cur_group_; // 当前创建的是第几组
    bool initialized_;
};

#endif