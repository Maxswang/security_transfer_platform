#ifndef MYSQL_CONNECTION_POOL_H_
#define MYSQL_CONNECTION_POOL_H_

#include <string>
#include <list>

//#include "muduo/base/Mutex.h"
//#include "muduo/base/CurrentThread.h"

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <cppconn/driver.h>
#include <cppconn/connection.h>

#include "singleton.h"

class MySQLConnectionPool : boost::noncopyable
{
public:
    typedef std::list<sql::Connection*> ConnectionList;
    typedef ConnectionList::iterator ConnectionListIter;
    
    friend class Singleton<MySQLConnectionPool>;
    
    static MySQLConnectionPool& GetInstance();

    // 创建连接数为最大连接数一半的连接
    void InitConnectionPool(const std::string& ip, int port, const std::string& user, const std::string& password, 
                            const std::string& database, int max_pool_size, int init_size);
    
    sql::Connection* GetConnection();
    
    // 把连接放回池中
    void ReleaseConnection(sql::Connection * connection);
    
private:
    MySQLConnectionPool();
	~MySQLConnectionPool();
    
    sql::Connection* CreateConnection();
    void DestroyConnectionPool();
    void DestroyConnection(sql::Connection * connection);

    std::string ip_;
    int port_;
    std::string user_;
    std::string password_;
    std::string database_;
    
    int max_pool_size_;
    int current_pool_size_;
    
    ConnectionList idle_conns_;
    ConnectionList occupiped_conns_;
    
    sql::Driver * driver_;
    boost::mutex mutex_;
//        muduo::MutexLock mutex_;
};

#endif // MYSQL_CONNECTION_POOL_H_
