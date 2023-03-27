//
// Created by cyh on 2021/4/29.
//

#ifndef NETLIB_ACCEPTOR_H
#define NETLIB_ACCEPTOR_H

#include "Base/Noncopyable.h"
#include "Net/EventLoop.h"
#include "Net/Channel.h"
#include "InetAddress.h"
#include "Socket.h"

class Acceptor : Noncopyable {
public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& addr);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { m_newConnectionCallback = cb; }

    void listen();

private:
    void handleRead();

private:
    EventLoop* m_loop;
    Socket m_acceptSocket;
    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
};

#endif //NETLIB_ACCEPTOR_H
