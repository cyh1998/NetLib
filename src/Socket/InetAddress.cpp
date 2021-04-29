//
// Created by cyh on 2021/4/28.
//

#include <endian.h>
#include <arpa/inet.h> //inet_pton()
#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, bool ifLoopback) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    in_addr_t ipType = ifLoopback ? INADDR_LOOPBACK : INADDR_ANY;
    m_addr.sin_addr.s_addr = htobe32(ipType);
    m_addr.sin_port = htobe16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr);
    m_addr.sin_port = htobe16(port);
}

const struct sockaddr *InetAddress::getSockAddr() const {
    return (const struct sockaddr*)&m_addr;
    // return reinterpret_cast<const struct sockaddr*>(&m_addr);
}

void InetAddress::setSockAddr(struct sockaddr_in& addr) {
    addr = m_addr;
}
