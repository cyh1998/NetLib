//
// Created by cyh on 2021/4/19.
//

#ifndef NETLIB_EPOLLER_H
#define NETLIB_EPOLLER_H

#include <vector>
#include <map>
#include "../Base/Noncopyable.h"
#include "EventLoop.h"

class Channel;
struct epoll_event;

class Epoller : Noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    explicit Epoller(EventLoop* loop);
    ~Epoller();

    void poll(int timeoutMs, ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void asserInLoopThread() { m_ownerLoop->asserInLoopThread(); }

private:
    void update(int operation, Channel* channel) const;
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

private:
    static const int maxEventSize = 16;

    int m_epollFd;
    EventLoop* m_ownerLoop;
    std::vector<struct epoll_event> m_events;
    std::map<int, Channel*> m_channelMap;
};


#endif //NETLIB_EPOLLER_H
