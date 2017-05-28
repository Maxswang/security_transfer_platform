#include "stp_dao.h"
#include "config_parser.h"
#include "stputil/mysql_connection_pool.h"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <glog/logging.h>

#include <stdio.h>
#include <stdlib.h>

#define SAFE_DELETE(ptr) \
    if (ptr != NULL) \
    { \
        delete ptr; \
        ptr = NULL; \
    }

StpDao::StpDao() 
    : driver_(::get_driver_instance())
{
    
}

StpDao::~StpDao()
{
    
}

StpDao &StpDao::GetInstance()
{
    static StpDao s_Instance;
    return s_Instance;
}

bool StpDao::CheckStpGuidValid(uint64_t stp_guid)
{
    MySQLConnectionPool& pool = MySQLConnectionPool::GetInstance();
    bool res = false;
    try
    {
        
        sql::Connection * conn = pool.GetConnection();
        if (conn == NULL)
        {
            LOG(ERROR) << "get connection failed!";
            return false;
        }
        
        sql::PreparedStatement* stmt = conn->prepareStatement("select stp_guid from stp_deploy where stp_guid=?;");
        if (stmt == NULL)
        {
            LOG(ERROR) << "prepare stmt failed!";
            pool.ReleaseConnection(conn);
            return false;
        }
        
        stmt->setInt64(1, stp_guid);
        
        sql::ResultSet* result = stmt->executeQuery();
        if (result != NULL && result->next())
        {
            res = true;
        }
        SAFE_DELETE(result);
        SAFE_DELETE(stmt);
        pool.ReleaseConnection(conn);
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "catch db exception, reason is " << e.what();
    }
    
    return res;
}
