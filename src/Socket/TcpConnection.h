//
// Created by cyh on 2022/1/25.
//

#ifndef NETLIB_TCPCONNECTION_H
#define NETLIB_TCPCONNECTION_H

#include <string>
#include "Buffer.h"
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
    EventLoop* getLoop() const { return m_loop; }
    const std::string& name() const { return m_name; }
    bool connected() const { return m_state == kConnected; }

    void send(const std::string & message);
    void shutdown();

    void setConnectionCallback(const ConnectionCallback& cb)
    { m_connectionCallback = cb; }

    void setCloseCallback(const CloseCallback& cb)
    { m_closeCallback = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { m_messageCallback = cb; }

    void connectEstablished();
    void connectDestroyed();

private:
    enum StateE { kDisconnecting, kDisconnected, kConnecting, kConnected };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string & message);
    void shutdownInLoop();

    void setState(StateE s) { m_state = s; }

private:
    EventLoop* m_loop;
    const std::string m_name;
    StateE m_state;
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Channel> m_channel;
    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    CloseCallback m_closeCallback;
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
};


#endif //NETLIB_TCPCONNECTION_H
