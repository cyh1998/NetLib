//
// Created by cyh on 2022/1/25.
//

#include <functional>
#include "TcpConnection.h"
#include "Log/Log.h"
#include "Net/Channel.h"
#include "Net/EventLoop.h"
#include "Socket/Socket.h"
#include "Socket/SocketOps.h"

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr) :
    m_loop(loop),
    m_name(name),
    m_state(kConnecting),
    m_socket(new Socket(sockfd)),
    m_channel(new Channel(loop, sockfd)),
    m_localAddr(localAddr),
    m_peerAddr(peerAddr)
{
    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    m_socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {

}

void TcpConnection::send(const std::string &message) {
    if (m_state == kConnected) {
        if (m_loop->isInLoopThread()) {
            sendInLoop(message);
        } else {
            m_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::shutdown() {
    if (m_state == kConnected) {
        setState(kDisconnecting);
        m_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    m_socket->setTcpNoDelay(on);
}


void TcpConnection::connectEstablished() {
    m_loop->assertInLoopThread();
    setState(kConnected);
    m_channel->enableReading();

    m_connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    m_loop->assertInLoopThread();
    if (kConnected == m_state || kDisconnecting == m_state) {
        setState(kDisconnected);
        m_channel->disableAll();

        m_connectionCallback(shared_from_this());
    }
    m_channel->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    int savedErrno = 0;
    ssize_t n = m_inputBuffer.readFd(m_channel->fd(), savedErrno);
    if (n > 0) {
        m_messageCallback(shared_from_this(), &m_inputBuffer, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        handleError();
    }
}

void TcpConnection::handleWrite() {
    m_loop->assertInLoopThread();
    if (m_channel->isWriting()) {
        ssize_t nwrote = sockets::write(m_channel->fd(),
                                        m_outputBuffer.peek(),
                                        m_outputBuffer.readableBytes());

        if (nwrote > 0) {
            m_outputBuffer.retrieve(nwrote);
            if (m_outputBuffer.readableBytes() == 0) {
                m_channel->disableWriting();

                // writeCompleteCallback
                // ...

                if (kDisconnecting == m_state) {
                    shutdownInLoop();
                }
            } else {
                LOG_INFO("more data to write");
            }
        } else {
            LOG_ERROR("handleWrite error");
        }
    } else {
        LOG_INFO("Connection fd is down");
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();
    setState(kDisconnected);
    m_channel->disableAll();
    m_connectionCallback(shared_from_this());
    m_closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
    //...
}

void TcpConnection::sendInLoop(const std::string &message) {
    m_loop->assertInLoopThread();
    ssize_t nworte = 0;
    size_t remaining = message.size();
    if (!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0) {
        nworte = sockets::write(m_channel->fd(), message.data(), message.size());
        if (nworte >= 0) {
            remaining = remaining - nworte;
            if (remaining == 0) {
                // writeCompleteCallback
            }
        } else {
            nworte = 0;
            if (errno != EWOULDBLOCK) {
                // send error
            }
        }
    }

    if (remaining > 0) {
        m_outputBuffer.append(message.data() + nworte, message.size() - nworte);
        if (!m_channel->isWriting()) {
            m_channel->enableWriting();
        }
    }
}

void TcpConnection::shutdownInLoop() {
    m_loop->assertInLoopThread();
    if (!m_channel->isWriting()) {
        m_socket->shutdownWrite();
    }
}


