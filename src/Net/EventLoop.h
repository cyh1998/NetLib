//
// Created by cyh on 2021/4/19.
//

#ifndef NETLIB_EVENTLOOP_H
#define NETLIB_EVENTLOOP_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <vector>
#include <memory> //unique_ptr
#include "../Base/Noncopyable.h"
#include "Channel.h"

class Epoller;

using Functor = std::function<void()>;
using ChannelList = std::vector<Channel*>;

class EventLoop : Noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    static EventLoop* getEventLoopOfCurrentThread();

    void loop();
    void quit();
    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNoInLoopThread();
        }
    }

    void runInLoop(Functor cb);

    bool isInLoopThread() const { return m_threadId == syscall(SYS_gettid); }
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

private:
    void abortNoInLoopThread();

private:
    bool m_looping;
    bool m_quit;
    const pid_t m_threadId;
    std::unique_ptr<Epoller> m_epoller;
    ChannelList m_activeChannels;
};

#endif //NETLIB_EVENTLOOP_H
