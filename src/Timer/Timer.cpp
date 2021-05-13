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
