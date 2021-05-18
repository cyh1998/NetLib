//
// Created by cyh on 2021/4/19.
//

#include "EventLoop.h"
#include "../Log/Log.h"
#include "Epoller.h"

namespace {
    __thread EventLoop* t_loopInThisThread = nullptr;
    const int kEpollTimeMs = 10000;
}

EventLoop::EventLoop() :
    m_looping(false),
    m_quit(false),
    m_threadId(syscall(SYS_gettid)),
    m_epoller(std::make_unique<Epoller>(this))
{
    if (t_loopInThisThread) {
        LOG_ERROR("EventLoop is exists in this thread, thread_id is %d", m_threadId);
        // ...
    } else {
        LOG_INFO("EventLoop created, thread_id is %d", m_threadId);
        t_loopInThisThread = this;
    }
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
        }
    }

    LOG_INFO("EventLoop stop looping");
    m_looping = false;
}

void EventLoop::quit() {
    m_quit = true;
}

void EventLoop::abortNoInLoopThread() {
    LOG_INFO("EventLoop abortNoInLoopThread");
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

