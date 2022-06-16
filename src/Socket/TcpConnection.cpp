//
// Created by cyh on 2022/1/25.
//

#include <functional>
#include "TcpConnection.h"
#include "../Log/Log.h"
#include "../Net/Channel.h"
#include "../Net/EventLoop.h"
#include "../Socket/Socket.h"
#include "../Socket/SocketOps.h"

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr) :
    m_loop(loop),
    m_name(name),
    m_state(kConnecting),
    m_socket(new Socket(sockfd)),
    m_channel(new Channel(loop, sockfd)),
    m_localAddr(localAddr),
    m_peerAddr(peerAddr)
{
    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {

}

void TcpConnection::connectEstablished() {
    m_loop->assertInLoopThread();
    setState(kConnected);
    m_channel->enableReading();

    m_connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    m_loop->assertInLoopThread();
    if (kConnected == m_state) {
        setState(kDisconnected);
        m_channel->disableAll();

        m_connectionCallback(shared_from_this());
    }
    m_channel->remove();
}

void TcpConnection::handleRead() {
    char buf[256];
    ssize_t n = sockets::read(m_channel->fd(), &buf, sizeof(buf));
    if (n > 0) {
        m_messageCallback(shared_from_this(), buf, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleWrite() {

}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();
    m_channel->disableAll();
    m_closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
    //...
}

