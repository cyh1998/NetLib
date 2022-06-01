//
// Created by cyh on 2021/4/30.
//

#ifndef NETLIB_TCPSERVER_H
#define NETLIB_TCPSERVER_H

#include <map>
#include <memory>

#include "TcpConnection.h"

class Acceptor;
class EventLoop;
class InetAddress;

using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

class TcpServer : Noncopyable
{
public:
    TcpServer(EventLoop* loop, const InetAddress& addr, std::string name);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) {
        m_connectionCallback = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

private:
    EventLoop* m_loop;
    std::unique_ptr<Acceptor> m_acceptor;

    const std::string m_name;
    int m_nextConnId;
    ConnectionCallback m_connectionCallback;
    ConnectionMap m_connections;
};

#endif //NETLIB_TCPSERVER_H
