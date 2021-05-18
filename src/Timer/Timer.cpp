//
// Created by cyh on 2021/5/13.
//

#include "Timer.h"

Timer::Timer(TimerCallback cb, Timestamp when, double interval) :
    m_callback(std::move(cb)),
    m_expiration(when),
    m_interval(interval),
    m_repeat(interval > 0)
{
}

void Timer::restart(Timestamp now) {
    if (m_repeat) {
        m_expiration = AddTime(now, m_interval);
    } else {
        m_expiration = Timestamp::invalid();
    }
}

