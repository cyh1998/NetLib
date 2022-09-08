//
// Created by cyh on 2021/4/29.
//

#include <sys/uio.h> //readv
#include "../Log/Log.h"
#include "SocketOps.h"

void sockets::bind(int sockfd, const struct sockaddr* addr) {
    int ret = ::bind(sockfd, addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        LOG_ERROR("Bind error!")
    }
}

void sockets::listen(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG_ERROR("Listen error!")
    }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof (*addr));
    int connfd = ::accept4(sockfd, (struct sockaddr*)addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        //...
    }
    return connfd;
}

ssize_t sockets::read(int sockfd, void *buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
    return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd) {
    int ret = ::close(sockfd);
    if (ret < 0) {
        LOG_ERROR("Close error!")
    }
}

void sockets::shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOG_ERROR("sockets::shutdownWrite");
    }
}

int sockets::createNonblockingSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    if (sockfd < 0) {
        LOG_ERROR("Create Nonblocking Socket error!")
    }
    return sockfd;
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd) {
    sockaddr_in6 localaddr{};
    memset(&localaddr, 0, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof (localaddr));
    if (::getsockname(sockfd, (struct sockaddr*)&localaddr, &addrlen) < 0)
    {
        LOG_ERROR("GetLocalAddr error!")
    }
    return localaddr;
}
