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
#include "Singleton.h"

class SqlConnPool {
public:
    SqlConnPool();
    ~SqlConnPool();

    void Init(const std::string& host, int port,
              const std::string& user, const std::string& pwd,
              const std::string& db_name, int conn_size);
    void ClosePool();

    MYSQL *GetConnObj();
    void FreeConnObj(MYSQL *conn);

private:
    std::queue<MYSQL *> m_connQue;

    int m_max_conn;
    std::mutex m_mutex;
    // sem_t m_semId;
};

using g_SqlConnPoolMgr = Singleton<SqlConnPool>;

#endif //NETLIB_SQLCONNPOOL_H
