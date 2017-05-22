#include "stp_dao.h"
#include "config_parser.h"
#include "stputil/mysql/mysql_connection.h"
#include "stputil/mysql/prepared_statement.h"
#include "stputil/mysql/resultset.h"

#include <glog/logging.h>

#define SAFE_DELETE(ptr) \
    if (ptr != NULL) \
    { \
        delete ptr; \
        ptr = NULL; \
    }

bool StpDao::CheckStpGuidValid(uint64_t stp_guid)
{
    ConfigParser& parser = ConfigParser::GetInstance();
    bool res = false;
    try
    {
        MySQLConnection conn(parser.db_addr().c_str(),
                             parser.db_user().c_str(),
                             parser.db_passwd().c_str(),
                             parser.database().c_str(),
                             parser.db_port());
        
        if (!conn.Connect("utf8", 15, true))
        {
            LOG(ERROR) << "get db connection failed!";\
            return false;
        }
        
        PreparedStatement* stmt = conn.PrepareStatement("select * from stp_deploy where stp_guid=?;");
        if (stmt == NULL)
        {
            LOG(ERROR) << "prepare stmt failed!";\
            return false;
        }
        
        stmt->SetBigInt(0, stp_guid);
        
        ResultSet* result = stmt->ExecuteQuery();
        if (result != NULL && result->Next())
        {
            res = true;
        }
        
        conn.Close();
        
//        SAFE_DELETE(result);
//        SAFE_DELETE(stmt);
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "catch db exception, reason is " << e.what();
    }
    
    return res;
}
