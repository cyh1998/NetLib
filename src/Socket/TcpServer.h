//
// Created by cyh on 2021/4/30.
//

#ifndef NETLIB_TCPSERVER_H
#define NETLIB_TCPSERVER_H

#include <memory>
#include "../Base/Noncopyable.h"

class Acceptor;
class EventLoop;
class InetAddress;

class TcpServer : Noncopyable {
public:
    TcpServer(EventLoop* loop, const InetAddress& addr);
    ~TcpServer();

    void start();

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

private:
    EventLoop* m_loop;
    std::unique_ptr<Acceptor> m_acceptoe;
};

#endif //NETLIB_TCPSERVER_H
