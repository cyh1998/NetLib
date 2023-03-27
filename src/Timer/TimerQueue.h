//
// Created by cyh on 2021/5/17.
//

#ifndef NETLIB_TIMERQUEUE_H
#define NETLIB_TIMERQUEUE_H

#include <set>
#include <vector>
#include <utility> //make_pair()
#include "Base/Noncopyable.h"
#include "Net/EventLoop.h"
#include "Net/Channel.h"
#include "Timer.h"

class TimerQueue : Noncopyable {
public:
    typedef std::function<void()> TimerCallback;

    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

public:
    void addTimer(TimerCallback cb, Timestamp when, double interval);

private:
    typedef std::pair<Timestamp, std::shared_ptr<Timer>> Entry;
    // typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    void addTimerInLoop(Timer* timer);
    bool insert(Timer* timer);

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timers;
};

#endif //NETLIB_TIMERQUEUE_H
