//
// Created by cyh on 2021/4/29.
//

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
    socklen_t addrlen = sizeof(*addr);
    int connfd = ::accept4(sockfd, (struct sockaddr*)&addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        //...
    }
    return connfd;
}

void sockets::close(int sockfd) {
    int ret = ::close(sockfd);
    if (ret < 0) {
        LOG_ERROR("Close error!")
    }
}

int sockets::createNonblockingSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    if (sockfd < 0) {
        LOG_ERROR("Create Nonblocking Socket error!")
    }
    return sockfd;
}
