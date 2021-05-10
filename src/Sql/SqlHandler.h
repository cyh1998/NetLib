//
// Created by cyh on 2021/5/10.
//

#ifndef NETLIB_SQLHANDLER_H
#define NETLIB_SQLHANDLER_H

#include "SqlConnPool.h"

class SqlHandler {
public:
    SqlHandler(SqlConnPool *connpool);
    SqlHandler(MYSQL **sql, SqlConnPool *connpool);
    ~SqlHandler();

private:
    MYSQL *m_sql;
    SqlConnPool *m_connpool;
};

#endif //NETLIB_SQLHANDLER_H
