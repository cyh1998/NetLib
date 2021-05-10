//
// Created by cyh on 2021/5/10.
//

#include "SqlHandler.h"

SqlHandler::SqlHandler(SqlConnPool *connpool) {
    if (connpool) {
        m_sql = connpool->GetConnObj();
        m_connpool = connpool;
    }
}

SqlHandler::SqlHandler(MYSQL **sql, SqlConnPool *connpool) {
    if (connpool) {
        *sql = connpool->GetConnObj();
        m_sql = *sql;
        m_connpool = connpool;
    }
}

SqlHandler::~SqlHandler() {
    if (m_sql) {
        m_connpool->FreeConnObj(m_sql);
    }
}
