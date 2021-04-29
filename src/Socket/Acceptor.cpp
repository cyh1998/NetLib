//
// Created by cyh on 2021/4/29.
//

#include "SocketOps.h"
#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr) :
    m_loop(loop),
    m_acceptSocket(sockets::createNonblockingSocket()),
    m_acceptChannel(m_loop, m_acceptSocket.fd())
{
    m_acceptSocket.setAddrReusable(true);
    m_acceptSocket.bindAddress(addr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    m_acceptChannel.disableAll();
    // m_acceptChannel.remove();
}

void Acceptor::listen() {
    m_loop->assertInLoopThread();
    m_acceptSocket.listenAddress();
    m_acceptChannel.enableReading();
}

void Acceptor::handleRead() {
    m_loop->assertInLoopThread();
    InetAddress peerAddr;
    int connfd = m_acceptSocket.acceptAddress(&peerAddr);
    if (connfd >= 0) {
        if (m_newConnectionCallback) m_newConnectionCallback(connfd, peerAddr);
    } else {
        sockets::close(connfd);
    }
}
