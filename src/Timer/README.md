## 定时器
这里弃用了原先的三个定时器类：升序链表定时器 `TimerList`、时间堆定时器 `TimerHeap`、时间轮定时器 `TimerWheel`。  
这三个定时器类是我在学习《Linux高性能服务器编程》(游双 著)时，实现的一个聊天室服务器 [ChatRoomServer](https://github.com/cyh1998/ChatRoomServer) 中直接移植过来的。 
此定时器包含了用户数据类、定时器类、以及管理定时器的数据结构类，并配合服务端处理 `SIGALRM` 信号来实现定时机制
 
陈硕在《Linux多线程服务端编程》中提到：多线程服务器应该尽量避免使用信号。 
muduo源码中通过Linux提供的 `timerfd` 将时间变成了一个文件描述符，该描述符在定时器超时的那一刻变为可读，可以很方便的融入到select/poll/epoll中，统一事件源。 
 
因此，我参考muduo的实现方式，重构了定时器的实现。
