//
// Created by cyh on 2021/4/20.
//

#ifndef NETLIB_NONCOPYABLE_H
#define NETLIB_NONCOPYABLE_H

class Noncopyable
{
public:
    Noncopyable(const Noncopyable&) = delete;
    const Noncopyable& operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};

#endif //NETLIB_NONCOPYABLE_H
