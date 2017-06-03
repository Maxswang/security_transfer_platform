#include "stp_crypto_negotiate.h"
#include "stpserver/config_parser.h"

#include <glog/logging.h>


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
