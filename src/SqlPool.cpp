#include "SqlPool.hpp"
#include <stdexcept>
SqlPool *SqlPool::getInstance()
{
    static SqlPool instance;
    return &instance;
}

SqlPool::~SqlPool()
{
    UniqueLock lock(mtx_);
    while (!connQueue_.empty())
    {
        mysql_close(connQueue_.front());
        connQueue_.pop();
    }
}

void SqlPool::init(const std::string &host,
                   const std::string &user,
                   const std::string &password,
                   const std::string &dbName,
                   unsigned int port)
{
    UniqueLock lock(mtx_);

    host_ = host;
    user_ = user;
    password_ = password;
    dbName_ = dbName;
    port_ = port;

    while(currentConn_<MAX_CONN)
    {
        createConn();
    }
}

MYSQL *SqlPool::getConn()
{
    UniqueLock lock(mtx_);
    MYSQL *conn = nullptr;

    while (!connQueue_.empty() && mysql_ping(connQueue_.front()) != 0)
    {
        mysql_close(connQueue_.front());
        connQueue_.pop();
        --currentConn_;
    }

    if (!connQueue_.empty())
    {
        conn = connQueue_.front();
        connQueue_.pop();
        return conn;
    }

    if (currentConn_<MAX_CONN)
    {
        createConn();
        conn = connQueue_.front();
        connQueue_.pop();
        return conn;
    }

    condVar_.wait(lock, [this]() { return !connQueue_.empty(); });
    conn = connQueue_.front();
    connQueue_.pop();
    return conn;
}

void SqlPool::createConn()
{
    MYSQL *conn = mysql_init(nullptr);
    if (!conn)
    {
        throw std::runtime_error("MySQL initialization failed");
    }
    if (!mysql_real_connect(conn, host_.c_str(), user_.c_str(),
                            password_.c_str(), dbName_.c_str(),
                            port_, nullptr, 0))
    {
        mysql_close(conn);
        throw std::runtime_error("MySQL connection failed: " + std::string(mysql_error(conn)));
    }
    connQueue_.push(conn);
    ++currentConn_;
}

void SqlPool::releaseConn(MYSQL *conn)
{
    if (conn)
    {
        UniqueLock lock(mtx_);
        connQueue_.push(conn);
        condVar_.notify_one();
    }
}

SqlPool::QueryResult SqlPool::executeQuery(const std::string &query)
{
    MYSQL *conn = nullptr;
    ConnectionRAII RaII(&conn);
    if (!conn)
    {
        throw std::runtime_error("Failed to obtain a database connection");
    }

    if (mysql_query(conn, query.c_str()))
    {
        throw std::runtime_error("MySQL query failed: " + std::string(mysql_error(conn)));
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result)
    {
        throw std::runtime_error("MySQL store result failed: " + std::string(mysql_error(conn)));
    }

    QueryResult queryResult;
    MYSQL_FIELD *fields = mysql_fetch_fields(result);
    int numFields = mysql_num_fields(result);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 0; i < numFields; ++i)
        {
            queryResult[fields[i].name].emplace_back(row[i] ? row[i] : "NULL");
        }
    }

    mysql_free_result(result);
    return queryResult;
}

void SqlPool::execUpdate(const std::string &query)
{
    MYSQL *conn = nullptr;
    ConnectionRAII RaII(&conn);
    if (!conn)
    {
        throw std::runtime_error("Failed to obtain a database connection");
    }

    if (mysql_query(conn, query.c_str()))
    {
        throw std::runtime_error("MySQL update failed: " + std::string(mysql_error(conn)));
    }
}
