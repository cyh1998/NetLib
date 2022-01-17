//
// Created by cyh on 2021/5/8.
//

#include "../Log/Log.h"
#include "SqlConnPool.h"


SqlConnPool::SqlConnPool() {

}

SqlConnPool::~SqlConnPool() {
    ClosePool();
}

void SqlConnPool::Init(const std::string& host, int port, const std::string& user,
        const std::string& pwd, const std::string& db_name, int conn_size) {
    for (int i = 0; i < conn_size; i++) {
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if (!sql) {
            LOG_ERROR("MySql init error!");
        }

        sql = mysql_real_connect(sql, host.c_str(), user.c_str(), pwd.c_str(), db_name.c_str(), port, nullptr, 0);
        if (!sql) {
            LOG_ERROR("MySql Connect error!");
        }
        m_connQue.push(sql);
    }
    m_max_conn = conn_size;
    // sem_init(&m_semId, 0, m_max_conn);
}

MYSQL *SqlConnPool::GetConnObj() {
    MYSQL *sql = nullptr;
    if (m_connQue.empty()) {
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }
    // sem_wait(&m_semId);

    {
        std::lock_guard<std::mutex> locker(m_mutex);
        sql = m_connQue.front();
        m_connQue.pop();
    }

    return sql;
}

void SqlConnPool::FreeConnObj(MYSQL *conn) {
    if (conn != nullptr) {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_connQue.push(conn);
        // sem_post(&m_semId);
    }
}

void SqlConnPool::ClosePool() {
    std::lock_guard<std::mutex> locker(m_mutex);
    while(!m_connQue.empty()) {
        auto item = m_connQue.front();
        m_connQue.pop();
        mysql_close(item);
    }
    mysql_library_end();
}
