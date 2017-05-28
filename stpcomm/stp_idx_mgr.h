#ifndef STP_IDX_MGR_H_
#define STP_IDX_MGR_H_

#include <set>

class StpIdxMgr
{
public:
//    static StpIdxMgr& GetInstance();
    StpIdxMgr(int max_idx);
    ~StpIdxMgr();
    
    int PopIdx();
    
    bool PushIdx(int idx);
    
    int max_idx() const { return max_idx_; }
    
private:

    
//    StpIdxMgr(const StpIdxMgr&);
//    StpIdxMgr& operator=(const StpIdxMgr&);
    
private:
    int max_idx_;
    std::set<int> idx_set_;
};


#endif