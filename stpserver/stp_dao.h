#ifndef STP_DAO_H_
#define STP_DAO_H_

#include <stdint.h>
#include <string>

class StpDao
{
public:
    static StpDao& GetInstance();
    bool CheckStpGuidValid(uint64_t stp_guid);
    bool InsertCryptoStatus(uint64_t stp_guid, int32_t group, int32_t idx, int64_t expires, const std::string& key);
private:
    StpDao();
    ~StpDao();
    
    StpDao(const StpDao&);
    StpDao& operator=(const StpDao&);
};

#endif
