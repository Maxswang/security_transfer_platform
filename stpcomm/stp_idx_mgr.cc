#include "stp_idx_mgr.h"
#include <stdlib.h>

StpIdxMgr::StpIdxMgr(int max_idx)
    : max_idx_(max_idx)
{
    if (max_idx > 100000 || max_idx <= 0)
        abort();
    
    for (int i = 0; i < max_idx_; ++i)
    {
        idx_set_.insert(i);
    }
}

StpIdxMgr::~StpIdxMgr()
{
    idx_set_.clear();
}

//StpIdxMgr &StpIdxMgr::GetInstance()
//{
//    static StpIdxMgr s_Instance(ConfigParser::GetInstance().max_idx());
//    return s_Instance;
//}

int StpIdxMgr::PopIdx()
{
    if (idx_set_.empty())
        return -1;
    
    int index = (*idx_set_.begin());
    idx_set_.erase(index);
    return index;
}

bool StpIdxMgr::PushIdx(int idx)
{
    if (idx_set_.size() >= max_idx_ && idx < 0)
        return false;
    
    idx_set_.insert(idx);
    return true;
}
