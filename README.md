# NetLib
Linux下的C++网络库

## 环境
```
Linux  5.4.0
gcc    7.5.0
Ubuntu 18.04.4
```

## 功能
* 基于Epoll实现的Reactor模式
* 利用阻塞队列实现的异步日志模块
* 基于pthread封装实现的线程对象
* 实现模板类线程池与函数对象线程池
* 实现基于升序链表的定时器、时间轮定时器、时间堆定时器

## 目录
```
./
├── src
│   ├── Base
│   │   └── Noncopyable      不可拷贝基类
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
│   ├── Thread
│   │   ├── FuncThreadPool   函数对象线程池
│   │   ├── ThreadObject     线程对象
│   │   └── ThreadPool       模板类线程池
│   └── Timer (待更新)
│       ├── TimerHeap        时间堆
│       ├── TimerList        基于升序链表的定时器
│       └── TimerWheel       时间轮
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
