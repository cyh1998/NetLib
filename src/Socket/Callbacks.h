//
// Created by cyh on 2022/6/1.
//

#ifndef NETLIB_CALLBACKS_H
#define NETLIB_CALLBACKS_H

#include <functional>
#include <memory>
#include "Buffer.h"
#include "../Timer/Timer.h"

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;

using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)>;

#endif //NETLIB_CALLBACKS_H
