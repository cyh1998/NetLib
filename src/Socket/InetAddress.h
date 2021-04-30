//
// Created by cyh on 2021/4/28.
//

#ifndef NETLIB_INETADDRESS_H
#define NETLIB_INETADDRESS_H

#include <netinet/in.h> //uint16_t sockaddr_in sa_family_t
#include <string.h> //memset()
#include <string>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool ifLoopback = false);
    InetAddress(const std::string& ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in& addr);

    const struct sockaddr* getSockAddr() const;
    void setSockAddr(struct sockaddr_in& addr);

    sa_family_t family() const { return m_addr.sin_family; }
    std::string getIpStr() const;
    uint16_t getPort() const;

private:
    sockaddr_in m_addr;
};

#endif //NETLIB_INETADDRESS_H
