//
// Created by cyh on 2022/1/17.
//

#ifndef NETLIB_SINGLETON_H
#define NETLIB_SINGLETON_H

template<typename ClassType>
class Singleton
{
public:
    static ClassType * instance();

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
    ~Singleton() = default;
};

template<typename ClassType>
ClassType * Singleton<ClassType>::instance()
{
    static ClassType s_singleton;
    return &s_singleton;
}

#endif //NETLIB_SINGLETON_H
