//
// Created by cyh on 2021/5/17.
//

#include <sys/timerfd.h>
#include "../Log/Log.h"
#include "TimerQueue.h"

typedef std::pair<Timestamp, std::shared_ptr<Timer>> Entry;
// typedef std::pair<Timestamp, Timer*> Entry;

namespace {

int createTimerfd() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        // ...
    }
    return timerfd;
}

void readTimerfd(int timerfd, Timestamp now) {
    uint64_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof(howmany));
    LOG_INFO("TimerQueue: Read timerfd %d", howmany);
    if (n != sizeof howmany) {
        // ...
    }
}

struct timespec howMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.MicroSecondsSinceEpoch() - Timestamp::Now().MicroSecondsSinceEpoch();
    if (microseconds < 100) microseconds = 100;
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));

    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        // ...
    }
}

}

TimerQueue::TimerQueue(EventLoop *loop) :
    m_loop(loop),
    m_timerfd(createTimerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers()
{
    m_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue() {
    m_timerfdChannel.disableAll();
    m_timerfdChannel.remove();
    ::close(m_timerfd);
}

void TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    // m_loop->runInLoop();
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    m_loop->assertInLoopThread();

    if (insert(timer)) {
        resetTimerfd(m_timerfd, timer->expiration());
    }
}

bool TimerQueue::insert(Timer *timer) {
    m_loop->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = m_timers.begin();
    if (it == m_timers.end() || when < it->first){
        earliestChanged = true;
    }

    //std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when, timer));
    m_timers.emplace(std::make_pair(when, timer));

    return earliestChanged;
}

void TimerQueue::handleRead() {
    m_loop->assertInLoopThread();
    Timestamp now(Timestamp::Now());
    readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);
    for (const Entry &i : expired) {
        i.second->run();
    }

    reset(expired, now);
}

std::vector<Entry> TimerQueue::getExpired(Timestamp now) {
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = m_timers.lower_bound(sentry);

    std::vector<Entry> expired(m_timers.begin(), end);
    // std::copy(m_timers.begin(), end, back_inserter(expired));
    m_timers.erase(m_timers.begin(), end);

    return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {
    Timestamp nextExpire;

    for (const Entry &i : expired) {
        if (i.second->repeat()) {
            i.second->restart(now);
            insert(i.second.get());
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        resetTimerfd(m_timerfd, nextExpire);
    }
}
