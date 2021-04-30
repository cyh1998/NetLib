//
// Created by cyh on 2021/4/30.
//

#include "Acceptor.h"
#include "TcpServer.h"

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr) :
    m_loop(loop),
    m_acceptoe(new Acceptor(loop, addr))
{
    m_acceptoe->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {

}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {

}
