#include "stp_crypto_detail.h"
#include "stpserver/config_parser.h"


#include <glog/logging.h>
#include <stdlib.h>

bool CryptoItem::GenerateRandomKey()
{
    LOG(INFO) << "old key is " << key ;
    memset(key, 0, sizeof(key));
    const char* key_tab = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int key_tab_len = strlen(key_tab);
    
    srandom(time(NULL));
    
    for (int i = 0; i < 32; ++i)
    {
        key[i] = key_tab[random() % key_tab_len];
    }
    LOG(INFO) << "new key is " << key ;
    return true;
}

CryptoGroup::CryptoGroup(int g) 
    : group(g), idx_mgr(ConfigParser::GetInstance().max_idx()) 
{}

CryptoItem *CryptoGroup::GetCryptoItemByIdx(int idx) 
{
    if (idx < 0 || idx > idx_mgr.max_idx())
        return NULL;
    
    CryptoItem* item = static_cast<CryptoItem*>(shm_addr + sizeof(CryptoItem) * idx);
    return item;
}

void *CryptoGroup::TryAttachShm(size_t size) 
{
    ConfigParser& config = ConfigParser::GetInstance();
    do
    {
        key_t key = ftok(config.path().c_str(), group);
        if (key == -1)
        {
            LOG(ERROR) << "ftok " << config.path() << " , group " << group << " failed!";
            break;
        }
        
        shmid = shm_oper.shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
        if (shmid != -1)
        {
            LOG(ERROR) << "try attach failed, because not created before!"; // TODO
            break;
        }
        
        if (errno != EEXIST)
        {
            LOG(ERROR) << "try attach failed, errno=" << errno << ", reason=" << strerror(errno);
            break;
        }
        
        shmid = shm_oper.shmget(key, size, IPC_CREAT | 0666);
        if (shmid == -1)
        {
            LOG(ERROR) << "try attach failed!";
            break;
        }
        
        void* addr = shm_oper.shmat(shmid, NULL, 0);
        if (addr != NULL)
        {
            LOG(INFO) << "try attach succeed!";
            return addr;
        }
        LOG(ERROR) << "try attach failed, errno=" << errno << ", reason=" << strerror(errno);
    } while (false);
    
    if (shmid != -1)
    {
        shm_oper.shmctl(shmid, IPC_RMID, NULL);
    }
    return NULL;
}

bool CryptoGroup::CreateShm(size_t size) 
{
    ConfigParser& config = ConfigParser::GetInstance();
    key_t key = ftok(config.path().c_str(), group);
    if (key == -1)
    {
        LOG(ERROR) << "ftok " << config.path() << " , group " << group << " failed!";
        return false;
    }
    
    shmid = shm_oper.shmget(key, size, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        LOG(ERROR) << "shmget failed!";
        return false;
    }
    
    shm_addr = shm_oper.shmat(shmid, NULL, 0);
    if (shm_addr == NULL)
    {
        shm_oper.shmctl(shmid, IPC_RMID, NULL);
        LOG(ERROR) << "CreateShm failed, errno=" << errno << ", reason=" << strerror(errno);
        return false;
    }
    
    LOG(INFO) << "CreateShm succeed, shmid=" << shmid << ", addr=" << shm_addr;
    return true;
}
