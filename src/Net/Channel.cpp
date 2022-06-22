//
// Created by cyh on 2021/4/23.
//

#include <sys/epoll.h>
#include "EventLoop.h"
#include "Channel.h"

const uint32_t Channel::s_noneEvent = 0;
const uint32_t Channel::s_readEvent = EPOLLIN | EPOLLPRI;
const uint32_t Channel::s_writeEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd) :
    m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{

}

Channel::~Channel() {

}

void Channel::handleEvent(Timestamp receiveTime) {
    // 触发断开事件
    if ((m_revents & EPOLLRDHUP) && !(m_revents & EPOLLIN)) {
        if (m_closeCallback) m_closeCallback();
    }
    // 触发错误事件
    if (m_revents & EPOLLERR) {
        if (m_errorCallback) m_errorCallback();
    }
    // 触发可读事件
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (m_readCallback) m_readCallback(receiveTime);
    }
    // 触发可写事件
    if (m_revents & EPOLLOUT) {
        if (m_writeCallback) m_writeCallback();
    }
}

void Channel::remove() {
    if (isNoneEvent()) {
        m_loop->removeChannel(this);
    }
}

void Channel::update() {
    m_loop->updateChannel(this);
}
