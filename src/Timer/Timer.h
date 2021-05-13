//
// Created by cyh on 2021/5/13.
//

#ifndef NETLIB_TIMER_H
#define NETLIB_TIMER_H

#include <stdint.h>
#include <sys/time.h> //timeval gettimeofday()
#include <functional>
#include "../Base/Noncopyable.h"

class Timestamp {
public:
    Timestamp() : m_microSecondsSinceEpoch(0) {}
    explicit Timestamp(int64_t time) : m_microSecondsSinceEpoch(time) {}

    static Timestamp Now() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        int64_t seconds = tv.tv_sec;
        return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
    }

    int64_t MicroSecondsSinceEpoch() const { return m_microSecondsSinceEpoch; }

public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t m_microSecondsSinceEpoch;
};

inline Timestamp AddTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.MicroSecondsSinceEpoch() + delta);
}

class Timer : Noncopyable {
public:
    typedef std::function<void()> TimerCallback;

    Timer(TimerCallback cb, Timestamp when, double interval);

    void run() const { m_callback(); }

    Timestamp expiration() const  { return m_expiration; }
    bool repeat() const { return m_repeat; }

private:
    const TimerCallback m_callback;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
};

#endif //NETLIB_TIMER_H
