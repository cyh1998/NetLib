//
// Created by cyh on 2021/4/19.
//

#include "Epoller.h"
#include <sys/epoll.h>
#include <unistd.h> // close()
#include "../Log/Log.h"
#include "Channel.h"

Epoller::Epoller(EventLoop* loop) :
    m_ownerLoop(loop),
    m_epollFd(epoll_create(EPOLL_CLOEXEC)),
    m_events(maxEventSize)
{
    if (m_epollFd < 0) {
        LOG_ERROR("epoll_create error!");
    }
}

Epoller::~Epoller() {
    close(m_epollFd);
}

void Epoller::poll(int timeoutMs, ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(m_epollFd, m_events.data(), static_cast<int>(m_events.size()), timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0) {
        LOG_INFO("%d events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == m_events.size()) {
            m_events.resize(m_events.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG_INFO("no events happened");
    } else {
        if (savedErrno != EINTR) {
            errno = savedErrno;
            LOG_ERROR("epoll_wait error!");
        }
    }
}

void Epoller::updateChannel(Channel *channel) {
    Epoller::assertInLoopThread();
    const int index = channel->getIndex();
    int fd = channel->fd();
    LOG_INFO("Epoller update: fd = %d events = %d index = %d", fd, channel->getEvents(), index);
    if (index == -1 || index == 2) {
        if (index == -1) {
            m_channelMap[fd] = channel;
        }

        channel->setIndex(1);
        update(EPOLL_CTL_ADD, channel);
    } else {
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::removeChannel(Channel *channel) {
    Epoller::assertInLoopThread();
    int fd = channel->fd();
    int index = channel->getIndex();
    LOG_INFO("Epoller remove: fd = %d", fd);
    m_channelMap.erase(fd);
    if (index == 1) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(-1);
}

void Epoller::update(int operation, Channel* channel) const {
    epoll_event event;
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int fd = channel->fd();
    epoll_ctl(m_epollFd, operation, fd, &event);
}

void Epoller::fillActiveChannels(int numEvents, Epoller::ChannelList *activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        auto* channel = static_cast<Channel*>(m_events[i].data.ptr);
        // int fd = channel->fd();
        // auto const it = m_channelMap.find(fd);
        channel->setRevents(m_events[i].events);
        activeChannels->push_back(channel);
    }
}
