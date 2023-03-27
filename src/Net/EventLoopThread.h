//
// Created by cyh on 2021/4/28.
//

#ifndef NETLIB_EVENTLOOPTHREAD_H
#define NETLIB_EVENTLOOPTHREAD_H

#include <functional>
#include <mutex>
#include <condition_variable>
#include "Base/Noncopyable.h"
#include "Thread/ThreadObject.h"

class EventLoop;

class EventLoopThread : Noncopyable {
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    explicit EventLoopThread(ThreadInitCallback cb = ThreadInitCallback());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop* m_loop;
    ThreadObject m_thread;
    ThreadInitCallback m_callback;
    std::mutex m_mutex;
    std::condition_variable m_condition;

};

#endif //NETLIB_EVENTLOOPTHREAD_H
