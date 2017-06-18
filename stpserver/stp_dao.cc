#include "stp_dao.h"
#include "config_parser.h"
#include "stputil/mysql/connection_pool.h"
#include "stputil/mysql/connection.h"
#include "stputil/mysql/statement.h"
#include "stputil/mysql/resultset.h"

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
    mysql::ConnectionPool& pool = mysql::ConnectionPool::GetInstance();
    bool res = false;
    try
    {
        
        mysql::Connection* conn = pool.GetConnection();
        if (conn == NULL)
        {
            LOG(ERROR) << "get connection failed!";
            return false;
        }
        
        mysql::PreparedStatement* stmt = conn->PrepareStatement("select stp_guid from stp_deploy where stp_guid=?;");
        if (stmt == NULL)
        {
            LOG(ERROR) << "prepare stmt failed!";
            pool.ReleaseConnection(conn);
            return false;
        }
        
        stmt->SetInt64(0, stp_guid);
        
        mysql::ResultSet* result = stmt->ExecuteQuery();
        if (result != NULL && result->Next())
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

bool StpDao::InsertCryptoStatus(uint64_t stp_guid, int32_t group, int32_t idx, int64_t expires, const std::string &key)
{
    mysql::ConnectionPool& pool = mysql::ConnectionPool::GetInstance();
    bool res = false;
    try
    {
        
        mysql::Connection* conn = pool.GetConnection();
        if (conn == NULL)
        {
            LOG(ERROR) << "get connection failed!";
            return false;
        }
    
        std::string sql = "insert into stp_status(stp_guid, stp_key, expires, stp_group, idx, insert_time, stp_desc) values(?,?,?,?,?,now(),?);";
        mysql::PreparedStatement* stmt = conn->PrepareStatement(sql);
        if (stmt == NULL)
        {
            LOG(ERROR) << "prepare stmt failed!";
            pool.ReleaseConnection(conn);
            return false;
        }
        stmt->SetInt64(0, stp_guid);
        stmt->SetString(1, key.c_str());
        stmt->SetInt(2, group);
        stmt->SetInt(3, idx);
        stmt->SetInt64(4, expires);
        stmt->SetString(5, "密钥协商");

        if (stmt->ExecuteUpdate() != -1)
        {
            res = true;
        }
        SAFE_DELETE(stmt);
        pool.ReleaseConnection(conn);
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "catch db exception, reason is " << e.what();
    }
    
    return res;;
}
