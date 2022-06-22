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
#include <mutex>
#include <functional>
#include "../Base/Noncopyable.h"
#include "../Timer/Timer.h"

class Epoller;
class Channel;

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
    void queueInLoop(Functor cb);

    void wakeup();

    bool isInLoopThread() const { return m_threadId == syscall(SYS_gettid); }
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

private:
    void abortNoInLoopThread();
    void handleRead();
    void doPendingFunctors();

private:
    bool m_looping;
    bool m_quit;
    bool m_callingPendingFunctors;

    const pid_t m_threadId;
    std::unique_ptr<Epoller> m_epoller;
    ChannelList m_activeChannels;
    Timestamp m_pollReturnTime;

    int m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChannel;

    std::mutex m_mutex;
    std::vector<Functor> m_pendingFunctors;
};

#endif //NETLIB_EVENTLOOP_H
