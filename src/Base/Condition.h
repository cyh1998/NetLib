//
// Created by cyh on 2022/1/21.
//

#ifndef NETLIB_CONDITION_H
#define NETLIB_CONDITION_H

#include <errno.h>
#include "MutexLock.h"
#include <stdint.h>

class Condition : Noncopyable
{
public:
    explicit Condition(MutexLock & mutex) : m_mutex(mutex) {
        ::pthread_cond_init(&m_cond, nullptr);
    }
    ~Condition() {
        ::pthread_cond_destroy(&m_cond);
    }

    void wait() {
        ::pthread_cond_wait(&m_cond, m_mutex.getMutex());
    }
    bool waitForTime(unsigned int seconds) {
        timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);

        const int64_t kNanoSecondsForSecond = 1000000000;
        int64_t nanoseconds = seconds * kNanoSecondsForSecond;

        abstime.tv_sec += (nanoseconds + abstime.tv_nsec) / kNanoSecondsForSecond;
        abstime.tv_nsec = (nanoseconds + abstime.tv_nsec) % kNanoSecondsForSecond;

        return ETIMEDOUT == ::pthread_cond_timedwait(&m_cond, m_mutex.getMutex(), &abstime);
    }

    void notifyOne() {
        ::pthread_cond_signal(&m_cond);
    }
    void notifyAll() {
        ::pthread_cond_broadcast(&m_cond);
    }

private:
    MutexLock & m_mutex;
    pthread_cond_t m_cond;
};

#endif //NETLIB_CONDITION_H
