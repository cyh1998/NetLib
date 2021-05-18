//
// Created by cyh on 2021/4/23.
//

#ifndef NETLIB_CHANNEL_H
#define NETLIB_CHANNEL_H

#include <functional>
#include "../Base/Noncopyable.h"

class EventLoop;

class Channel : Noncopyable {
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);

    void handleEvent(); //处理文件描述符上的触发事件
    void setReadCallback(const EventCallback& cb) { m_readCallback = cb; }
    void setWriteCallback(const EventCallback& cb) { m_writeCallback = cb; }
    void setErrorCallback(const EventCallback& cb) { m_errorCallback = cb; }

    int fd() const { return m_fd; }
    int getEvents() const { return m_events; }
    void setRevents(int revt) { m_revents = revt; }
    bool isNoneEvent() const { return m_events == s_noneEvent; }
    
    void enableReading() { m_events |= s_readEvent; update(); }
    void enableWriting() { m_events |= s_writeEvent; update(); }
    void disableAll() { m_events = s_noneEvent; update(); }
    
    int getIndex() const { return m_index; }
    void setIndex(int idx) { m_index = idx; }

    EventLoop* ownerLoop() { return m_loop; }
    void remove();

private:
    void update();

private:
    static const uint32_t s_noneEvent;
    static const uint32_t s_readEvent;
    static const uint32_t s_writeEvent;

    EventLoop* m_loop;
    const int  m_fd;
    uint32_t   m_events;  //关心的IO事件
    uint32_t   m_revents; //当前活动的IO事件
    int        m_index;   //描述符的标识

    EventCallback m_readCallback;  //可读事件回调
    EventCallback m_writeCallback; //可写事件回调
    EventCallback m_errorCallback; //错误事件回调
};

#endif //NETLIB_CHANNEL_H
