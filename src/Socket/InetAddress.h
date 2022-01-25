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
    explicit InetAddress(uint16_t port = 0, bool ifLoopback = false, bool ipv6 = false);
    InetAddress(const std::string& ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr);
    explicit InetAddress(const struct sockaddr_in6& addr);

    const struct sockaddr* getSockAddr() const;
    void setSockAddr(struct sockaddr_in& addr);

    sa_family_t family() const { return m_addr.sin_family; }
    std::string getIpStr() const;
    uint16_t getPort() const;

private:
    union
    {
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
};

#endif //NETLIB_INETADDRESS_H
