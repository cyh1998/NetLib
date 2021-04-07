# NetLib
Linux下的C++网络库

## 功能
* 利用阻塞队列实现的异步日志模块
* 基于pthread封装实现的线程对象
* 实现模板类线程池与函数对象线程池
* 实现基于升序链表的定时器、时间轮定时器、时间堆定时器

## 目录
```
./
├── src
│   ├── Log
│   │   ├── BlockQueue      阻塞队列
│   │   └── Log             日志
│   ├── Thread
│   │   ├── FuncThreadPool  函数对象线程池
│   │   ├── ThreadObject    线程对象
│   │   └── ThreadPool      模板类线程池
│   └── Timer
│       ├── TimerHeap       时间堆
│       ├── TimerList       基于升序链表的定时器
│       └── TimerWheel      时间轮
├── main.cpp
└── README.md
```

## 编译
```shell script
mkdir bulid
cd bulid
cmake ..
make
```

## 致谢
《Linux高性能服务器编程》游双 著  
《Linux多线程服务端编程》陈硕 著
