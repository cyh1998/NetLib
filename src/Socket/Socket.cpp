//
// Created by cyh on 2021/4/28.
//

#include <arpa/inet.h>
#include "InetAddress.h"
#include "SocketOps.h"
#include "Socket.h"

Socket::Socket(int fd) :
    m_sockfd(fd)
{
}

Socket::~Socket() {
    sockets::close(m_sockfd);
}


void Socket::bindAddress(const InetAddress& addr) const {
    sockets::bind(m_sockfd, addr.getSockAddr());
}

void Socket::listenAddress() const {
    sockets::listen(m_sockfd);
}

int Socket::acceptAddress(InetAddress* peeraddr) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int connfd = sockets::accept(m_sockfd, &addr);
    if (connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::setAddrReusable(bool on) const {
    int optVal = on ? 1 : 0;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, static_cast<socklen_t>(sizeof optVal));
}

void Socket::setPortReusable(bool on) const {
    int optval = on ? 1 : 0;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::shutdownWrite() {
    sockets::shutdownWrite(m_sockfd);
}
