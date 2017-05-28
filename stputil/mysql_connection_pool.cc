#include "mysql_connection_pool.h"

#include <stdio.h>
#include <cppconn/exception.h>
#include <boost/thread/lock_guard.hpp>

MySQLConnectionPool::MySQLConnectionPool() : current_pool_size_(0), driver_(::get_driver_instance()) // unsafe
{
}

MySQLConnectionPool::~MySQLConnectionPool()
{
    DestroyConnectionPool();
}

MySQLConnectionPool& MySQLConnectionPool::GetInstance()
{
    return Singleton<MySQLConnectionPool>::GetInstance();
}

void MySQLConnectionPool::InitConnectionPool(const std::string &ip, int port, const std::string &user, 
                                             const std::string &password, const std::string &database, int max_pool_size, int init_size)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
//    muduo::MutexLockGuard guard(mutex_);
    
    ip_ = ip;
    port_ = port;
    user_ = user;
    password_ = password;
    database_ = database;
    max_pool_size_ = max_pool_size;
    
    assert(init_size <= max_pool_size_);
    
    sql::Connection * connection = NULL;
    while (current_pool_size_ < init_size)
    {
        if ((connection = CreateConnection()) != NULL)
        {
            idle_conns_.push_back(connection); 
            ++current_pool_size_;
        }
    }
}

sql::Connection *MySQLConnectionPool::GetConnection()
{
    boost::lock_guard<boost::mutex> guard(mutex_);
//    muduo::MutexLockGuard guard(mutex_);
    
    sql::Connection* connection = NULL;
    if (!idle_conns_.empty())
    {
        connection = idle_conns_.front();
        idle_conns_.pop_front();
        
        if (connection->isClosed())
        {
            delete connection;
            connection = CreateConnection();
        }
        if (connection == NULL)
            --current_pool_size_;
        else
            occupiped_conns_.push_back(connection);
        
        return connection;
    }
    else 
    {
         if (current_pool_size_ < max_pool_size_)
         {
             if ((connection = CreateConnection()) != NULL)
             {
                 ++current_pool_size_;
                 occupiped_conns_.push_back(connection);
             }
             return connection;
         }
         else
         {
             return NULL;
         }
    }
}

void MySQLConnectionPool::ReleaseConnection(sql::Connection *connection)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
//    muduo::MutexLockGuard guard(mutex_);
    
    if (connection != NULL)
    {
        occupiped_conns_.remove(connection);
        idle_conns_.push_back(connection);
    }
}

sql::Connection *MySQLConnectionPool::CreateConnection()
{
    sql::Connection * connection = NULL;
    
    char host[256] = {0};
    snprintf(host, 255, "tcp://%s:%d/%s", ip_.c_str(), port_, database_.c_str());
    
    try
    {
        connection = driver_->connect(host, user_, password_);
        if (connection != NULL)
        {
            // TODO 设置一定时间内断开
            connection->setClientOption("OPT_CHARSET_NAME", "utf8");
        }
    }
    catch (sql::SQLException & e)
    {
        fprintf(stderr, "%s\n", e.what());
    }
    
    return connection;
}

void MySQLConnectionPool::DestroyConnectionPool()
{
    ConnectionListIter iter = idle_conns_.begin();
    while (iter != idle_conns_.end())
    {
        DestroyConnection(*iter);
        ++iter;
    }
    iter = occupiped_conns_.begin();
    while (iter != occupiped_conns_.end())
    {
        DestroyConnection(*iter);
        ++iter;
    }
    
    idle_conns_.clear();
    occupiped_conns_.clear();
    current_pool_size_ = 0;
}

void MySQLConnectionPool::DestroyConnection(sql::Connection * connection)
{
    if (connection)
    {
        try
        {
            connection->close();
        }
        catch (sql::SQLException & e)
        {
            fprintf(stderr, "%s\n", e.what());
        }
        delete connection;
    }
}
