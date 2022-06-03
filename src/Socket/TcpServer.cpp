//
// Created by cyh on 2021/4/30.
//

#include <sstream>
#include <utility>
#include "Acceptor.h"
#include "TcpServer.h"
#include "SocketOps.h"

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr, std::string name) :
    m_loop(loop),
    m_acceptor(new Acceptor(loop, addr)),
    m_name(std::move(name)),
    m_started(false)
{
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1,
                                         std::placeholders::_2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if (!m_started) {
        m_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    m_loop->isInLoopThread();
    std::stringstream connName;
    connName << m_name << m_nextConnId;
    ++m_nextConnId;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    auto coon = std::make_shared<TcpConnection>(m_loop, connName.str(), sockfd,
                                                            localAddr, peerAddr);
    m_connections[connName.str()] = coon;
    coon->setConnectionCallback(m_connectionCallback);
    coon->connectEstablished();
}
