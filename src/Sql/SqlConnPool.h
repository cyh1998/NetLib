//
// Created by cyh on 2021/5/8.
//

#ifndef NETLIB_SQLCONNPOOL_H
#define NETLIB_SQLCONNPOOL_H

#include <string>
#include <queue>
#include <mutex>
// #include <semaphore.h>
#include <mysql/mysql.h>

class SqlConnPool {
public:
    static SqlConnPool *GetInstance();

    void Init(const std::string& host, int port,
              const std::string& user, const std::string& pwd,
              const std::string& db_name, int conn_size);
    void ClosePool();

    MYSQL *GetConnObj();
    void FreeConnObj(MYSQL *conn);

private:
    SqlConnPool();
    ~SqlConnPool();

private:
    std::queue<MYSQL *> m_connQue;

    int m_max_conn;
    std::mutex m_mutex;
    // sem_t m_semId;
};

#endif //NETLIB_SQLCONNPOOL_H
