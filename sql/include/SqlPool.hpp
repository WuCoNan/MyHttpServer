#pragma once
#include <mysql/mysql.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <unordered_map>
class SqlPool 
{
    friend class ConnectionRAII;
public:
    using QueryResult=std::unordered_map<std::string,std::vector<std::string>>;
    static SqlPool* getInstance();
    void init(const std::string& host,
              const std::string& user,
              const std::string& password,
              const std::string& dbName,
              unsigned int port=3306);
    QueryResult executeQuery(const std::string& query); 
    void execUpdate(const std::string& query);
private:
    SqlPool()=default;
    ~SqlPool();
    MYSQL* getConn();
    void releaseConn(MYSQL* conn);
    void createConn();
    
    using UniqueLock=std::unique_lock<std::mutex>;

    static constexpr std::size_t MAX_CONN=10;
    std::queue<MYSQL*> connQueue_;
    std::mutex mtx_;
    std::condition_variable condVar_;
    std::size_t currentConn_{0};

    std::string host_;
    std::string user_;
    std::string password_;
    std::string dbName_;
    unsigned int port_;

    class ConnectionRAII
    {
    public:
        ConnectionRAII(MYSQL** conn)
        {
            if(conn)
            {
                *conn=SqlPool::getInstance()->getConn();
                conn_=*conn;
            }
        }

        ~ConnectionRAII()
        {
            if(conn_)
            {
                SqlPool::getInstance()->releaseConn(conn_);
            }
        }
    private:
        MYSQL* conn_{nullptr};

    };
};