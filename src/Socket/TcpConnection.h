//
// Created by cyh on 2022/1/25.
//

#ifndef NETLIB_TCPCONNECTION_H
#define NETLIB_TCPCONNECTION_H

#include <string>
#include "../Base/Noncopyable.h"
#include "../Socket/Callbacks.h"
#include "../Socket/InetAddress.h"

class EventLoop;
class Channel;
class Socket;

class TcpConnection : Noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

public:
    void setConnectionCallback(const ConnectionCallback& cb)
    { m_connectionCallback = cb; }

private:
    enum StateE { kConnecting, kConnected };
    void handleRead();

private:
    EventLoop* m_loop;
    const std::string m_name;
    StateE m_state;
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Channel> m_channel;
    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;
    ConnectionCallback m_connectionCallback;
};


#endif //NETLIB_TCPCONNECTION_H
