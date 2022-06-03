//
// Created by cyh on 2022/1/25.
//

#include "TcpConnection.h"
#include "../Net/Channel.h"
#include "../Socket/Socket.h"
#include "../Net/EventLoop.h"

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

}

TcpConnection::~TcpConnection() {

}

void TcpConnection::connectEstablished() {
    m_loop->assertInLoopThread();
    setState(kConnected);
    m_connectionCallback(shared_from_this());
}

void TcpConnection::handleRead() {

}

