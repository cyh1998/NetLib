# NetLib
Linux 下的 C++ 网络库  
包括基本的网络模块、日志模块、线程模块、数据库模块等等

## 环境
```
Ubuntu 18.04.4
Linux  5.4.0
gcc    7.5.0
MySQL  5.7.31
```
## 功能
* 基于Epoll实现的Reactor模式
* 利用阻塞队列实现的异步日志模块
* 基于pthread封装实现的线程对象
* 模板类线程池与函数对象线程池
* 基于set实现的定时器容器
* 基于queue实现的简单数据库连接池

## 目录
```
./
├── src
│   ├── Base
│   │   ├── Condition        Condition封装
│   │   ├── MutexLock        Mutex封装
│   │   ├── Noncopyable      不可拷贝基类
│   │   └── Singleton        单例模板类
│   ├── Log
│   │   ├── BlockQueue       阻塞队列
│   │   └── Log              日志
│   ├── Net
│   │   ├── Channel          IO事件分发
│   │   ├── Epoller          IO多路复用
│   │   ├── EventLoop        事件循环
│   │   └── EventLoopThread  事件循环线程
│   ├── Socket
│   │   ├── Acceptor         连接接收类
│   │   ├── InetAddress      网络地址封装
│   │   ├── Socket           文件描述符 RAII 封装
│   │   ├── SocketOps        socket操作类
│   │   └── TcpServer        Tcp Server
│   ├── Sql
│   │   ├── SqlConnPool      数据库连接池
│   │   └── SqlHandler       数据库操作类
│   ├── Thread
│   │   ├── FuncThreadPool   函数对象线程池
│   │   ├── ThreadObject     线程对象
│   │   └── ThreadPool       模板类线程池
│   └── Timer
│       ├── Timer            定时器类
│       ├── TimerQueue       定时器容器类
│       ├── TimerHeap        时间堆定时器(弃用)
│       ├── TimerList        升序链表定时器(弃用)
│       └── TimerWheel       时间轮定时器(弃用)
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
