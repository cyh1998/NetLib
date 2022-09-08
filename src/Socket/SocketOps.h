//
// Created by cyh on 2021/4/29.
//

#ifndef NETLIB_SOCKETOPS_H
#define NETLIB_SOCKETOPS_H

#include <arpa/inet.h>
#include <unistd.h>

namespace sockets
{
    void bind(int sockfd, const struct sockaddr* addr);

    void listen(int sockfd);

    int accept(int sockfd, struct sockaddr_in* addr);

    ssize_t read(int sockfd, void *buf, size_t count);
    ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

    ssize_t write(int sockfd, const void *buf, size_t count);

    void close(int sockfd);

    void shutdownWrite(int sockfd);

    int  createNonblockingSocket();

    struct sockaddr_in6 getLocalAddr(int sockfd);
}

#endif //NETLIB_SOCKETOPS_H
