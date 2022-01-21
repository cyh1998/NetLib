//
// Created by cyh on 2022/1/20.
//

#ifndef NETLIB_MUTEXLOCK_H
#define NETLIB_MUTEXLOCK_H

#include <pthread.h>
#include "Noncopyable.h"

class MutexLock : Noncopyable
{
public:
    MutexLock() {
        ::pthread_mutex_init(&m_mutex, nullptr);
    }
    ~MutexLock() {
        ::pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        ::pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        ::pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_t * getMutex() {
        return &m_mutex;
    }

private:
    friend class Condition;

private:
    pthread_mutex_t m_mutex;
};

class MutexLockGuard : Noncopyable
{
public:
    MutexLockGuard(MutexLock & mutex) : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~MutexLockGuard() {
        m_mutex.unlock();
    }

private:
    MutexLock & m_mutex;
};

#endif //NETLIB_MUTEXLOCK_H
