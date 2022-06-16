//
// Created by cyh on 2021/4/19.
//

#include <sys/eventfd.h>
#include "EventLoop.h"
#include "../Log/Log.h"
#include "Epoller.h"
#include "../Socket/SocketOps.h"

namespace {
    __thread EventLoop* t_loopInThisThread = nullptr;
    const int kEpollTimeMs = 10000;

    int createEventfd() {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            LOG_ERROR("Failed in eventfd");
            abort();
        }
        return evtfd;
    }
}

EventLoop::EventLoop() :
    m_looping(false),
    m_quit(false),
    m_threadId(syscall(SYS_gettid)),
    m_epoller(std::make_unique<Epoller>(this)),
    m_wakeupFd(createEventfd()),
    m_wakeupChannel(new Channel(this, m_wakeupFd))
{
    if (t_loopInThisThread) {
        LOG_ERROR("EventLoop is exists in this thread, thread_id is %d", m_threadId);
        // ...
    } else {
        LOG_INFO("EventLoop created, thread_id is %d", m_threadId);
        t_loopInThisThread = this;
    }

    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
    t_loopInThisThread = nullptr;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::loop() {
    if (!m_looping) {
        assertInLoopThread();
        m_looping = true;
        m_quit = false;

        while (!m_quit) {
            m_activeChannels.clear();
            m_epoller->poll(kEpollTimeMs, &m_activeChannels);
            for (auto & item : m_activeChannels) {
                item->handleEvent();
            }
            doPendingFunctors();
        }
    }

    LOG_INFO("EventLoop stop looping");
    m_looping = false;
}

void EventLoop::quit() {
    m_quit = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_pendingFunctors.push_back(std::move(cb));
    }

    if (!isInLoopThread() || m_callingPendingFunctors) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    int n = sockets::write(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() writes %d bytes", n);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    if (channel->ownerLoop() == this) {
        assertInLoopThread();
        m_epoller->updateChannel(channel);
    }
}

void EventLoop::removeChannel(Channel *channel) {
    if (channel->ownerLoop() == this) {
        assertInLoopThread();
        m_epoller->removeChannel(channel);
    }
}

void EventLoop::abortNoInLoopThread() {
    LOG_INFO("EventLoop abortNoInLoopThread");
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    int n = sockets::read(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %d bytes", n);
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    m_callingPendingFunctors = true;

    {
        std::unique_lock<std::mutex> locker(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for (const Functor& functor : functors) {
        functor();
    }
    m_callingPendingFunctors = false;
}
