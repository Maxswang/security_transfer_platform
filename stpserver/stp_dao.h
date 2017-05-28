#ifndef STP_DAO_H_
#define STP_DAO_H_

#include <stdint.h>

#include <cppconn/connection.h>
#include <cppconn/driver.h>

class StpDao
{
public:
    static StpDao& GetInstance();
    bool CheckStpGuidValid(uint64_t stp_guid);
    
private:
    StpDao();
    ~StpDao();
    
    StpDao(const StpDao&);
    StpDao& operator=(const StpDao&);
    
    sql::Driver * driver_; // 
    
};

#endif
