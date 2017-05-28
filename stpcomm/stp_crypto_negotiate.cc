#include "stp_crypto_negotiate.h"
#include "stpserver/config_parser.h"
#include "stputil/shm.h"
#include <glog/logging.h>

#include <stdlib.h>

StpCryptoNegotiate &StpCryptoNegotiate::GetInstance()
{
    static StpCryptoNegotiate s_Instance;
    return s_Instance;
}

bool StpCryptoNegotiate::Init()
{
    if (initialized_)
        return initialized_;
    
    shm_map_.clear();
    
    const ConfigParser& config = ConfigParser::GetInstance();
    
    // 1、尝试连接共享内存
    int group = config.max_group();
    for (int i = 0; i < group; ++i)
    {
        CryptoGroup cg(i);
        void* shm_addr = cg.TryAttachShm(config.max_idx() * sizeof(CryptoItem));
        if (shm_addr != NULL)
        {
            cg.shm_addr = shm_addr;
            shm_map_.insert(std::make_pair(i, cg));
            cur_group_ = i;
            LOG(INFO) << "try attach shm in group " << i << " succeed!";
        }
    }
    
    // 2、没有创建共享内存，则创建一组
    if (shm_map_.empty())
    {
        CryptoGroup cg(0);
        if (!cg.CreateShm(config.max_idx() * sizeof(CryptoItem)))
        {
            LOG(ERROR) << "create a empty crypto group failed!" ;
            return false;
        }
        cur_group_ = 0;
        shm_map_.insert(std::make_pair(0, cg));
        LOG(INFO) << "create a empty crypto group succeed!" ;
    }
    
    LOG(INFO) << "StpCryptoNegotiate init succeed!";
    initialized_ = true;
    return initialized_;
}

bool StpCryptoNegotiate::CryptoNegotiate(int &group, int &idx, std::string &key)
{
    const ConfigParser& config = ConfigParser::GetInstance();
    if (cur_group_ < 0 || cur_group_ >= config.max_group())
    {
        LOG(ERROR) << "invalid cur group:" << cur_group_;
        return false;
    }
    
    CryptoGroup& cg = shm_map_[cur_group_];
    if (cg.group == -1)
    {
        LOG(ERROR) << "invalid cur group:" << cur_group_;
        return false;
    }
    
    // 首次协商
    if (group == -1 || idx == -1)
    {
        int unused_idx = cg.idx_mgr.PopIdx();
        if (unused_idx == -1)
        {
            ++cur_group_;
            cg = shm_map_[cur_group_];
            cg.group = group;
            unused_idx = cg.idx_mgr.PopIdx();
            if (!cg.CreateShm(config.max_idx() * sizeof(CryptoItem)))
            {
                LOG(ERROR) << "create a empty crypto group failed!" ;
                --cur_group_;
                cg.group = -1;
                return false;
            }
            else
            {
                CryptoItem* item = cg.GetCryptoItemByIdx(unused_idx);
                memset(item, 0x00, sizeof(CryptoItem));
                group = cur_group_;
                idx = unused_idx;
            }
        }
    }
    
    // 开始协商
    CryptoItem* item = cg.GetCryptoItemByIdx(idx);
    if (item == NULL)
    {
        LOG(ERROR) << "item can't be NULL";
        return false;
    }
    
    item->GenerateRandomKey();
    key = item->key;
    
    LOG(INFO) << "group is " << group << ", idx is " << idx;
    return true;
}

StpCryptoNegotiate::StpCryptoNegotiate()
    : cur_group_(0), initialized_(false)
{
    
}

StpCryptoNegotiate::~StpCryptoNegotiate()
{
    
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
