#ifndef MYSQL_CONNECTION_H_
#define MYSQL_CONNECTION_H_

#include "connection.h"
#include "mysql_buffer.h"
#include <vector>
#include <mysql/mysql.h>

class MySQLStatement;
class MySQLResultSet;
class MySQLPreparedStatement;
class MySQLPreparedResultSet;

class MySQLConnection : public Connection
{
    friend class MySQLStatement;
    friend class MySQLResultSet;
    friend class MySQLPreparedStatement;
    friend class MySQLPreparedResultSet;
public:
    MySQLConnection(const char* host, const char* user, const char* passwd, const char* database, 
                    short port, uint32_t param_buffer_size = 1024 * 1024, uint32_t result_buffer_size = 1024 * 1024);
    virtual ~MySQLConnection();
    
    virtual bool Connect(const char* charset, unsigned int timeout, bool auto_commit);
    virtual bool Reconnect();
    virtual bool IsConnected();
    
    virtual Statement*CreateStatement();
    virtual PreparedStatement* PrepareStatement(const std::string& sql);
    
    virtual void SetAutoCommit(bool auto_commit);
    virtual void Commit();
    virtual bool GetAutoCommit();
    
    virtual void Close();
    
    virtual int GetErrNo();
    virtual const char* GetError();
    
    MYSQL* GetMySQLHandler();
    
    MySQLBuffer* param_buffer() ;
    
    MySQLBuffer* result_buffer() ;
    
    
private:
    MYSQL mysql_;
    bool connected_;
    std::string host_;
    std::string user_;
    std::string passwd_;
    std::string db_;
    short port_;
    int ping_counter_;
    std::string charset_;
    unsigned int timeout_;
    bool auto_commit_;
    
    MySQLBuffer param_buffer_;
    MySQLBuffer result_buffer_;
};

#endif