//
// Created by cyh on 2021/4/28.
//

#ifndef NETLIB_INETADDRESS_H
#define NETLIB_INETADDRESS_H

#include <netinet/in.h> //uint16_t sockaddr_in
#include <string.h> //memset()
#include <string>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool ifLoopback = false);
    InetAddress(const std::string& ip, uint16_t port);

    const struct sockaddr* getSockAddr() const;
    void setSockAddr(struct sockaddr_in& addr);

private:
    sockaddr_in m_addr;
};

#endif //NETLIB_INETADDRESS_H
