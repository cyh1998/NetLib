//
// Created by cyh on 2021/4/28.
//

#include "EventLoop.h"
#include "EventLoopThread.h"

EventLoopThread::EventLoopThread(ThreadInitCallback cb) :
    m_loop(nullptr),
    m_thread(std::bind(&EventLoopThread::threadFunc, this)),
    m_callback(std::move(cb))
{
}

EventLoopThread::~EventLoopThread() {
    if (m_loop != nullptr) {
        m_loop->quit();
        m_thread.join();
    }
}

EventLoop *EventLoopThread::startLoop() {
    if (!m_thread.started()) {
        m_thread.start();

        {
            std::unique_lock<std::mutex> locker(m_mutex);
            while (m_loop == nullptr) {
                m_condition.wait(locker);
            }
        }

    }
    return m_loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (m_callback) {
        m_callback(&loop);
    }

    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_loop = &loop;
        m_condition.notify_all();
    }

    m_loop->loop();

    std::unique_lock<std::mutex> locker(m_mutex);
    m_loop = nullptr;
}
