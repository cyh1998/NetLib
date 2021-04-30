//
// Created by cyh on 2021/4/29.
//

#ifndef NETLIB_SOCKETOPS_H
#define NETLIB_SOCKETOPS_H

#include <arpa/inet.h>
#include <unistd.h>

namespace sockets {
    void bind(int sockfd, const struct sockaddr* addr);

    void listen(int sockfd);

    int  accept(int sockfd, struct sockaddr_in* addr);

    void close(int sockfd);

    int  createNonblockingSocket();
}

#endif //NETLIB_SOCKETOPS_H