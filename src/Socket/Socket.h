//
// Created by cyh on 2021/4/28.
//

#ifndef NETLIB_SOCKET_H
#define NETLIB_SOCKET_H

#include "../Base/Noncopyable.h"

class InetAddress;

class Socket : Noncopyable {
public:
    explicit Socket(int fd);

    int  fd() const { return m_sockfd; }

    void bindAddress(const InetAddress& addr) const;
    void listenAddress() const;
    int  acceptAddress(InetAddress* peeraddr);

    void setAddrReusable(bool on) const;
    void setPortReusable(bool on) const;

private:
    const int m_sockfd;
};

#endif //NETLIB_SOCKET_H
