set(base_SRCS
        Base/Noncopyable.h
        Log/BlockQueue.h
        Log/Log.h
        Log/Log.cpp
        Net/EventLoop.h
        Net/EventLoop.cpp
        Net/EventLoopThread.h
        Net/EventLoopThread.cpp
        Net/Channel.h
        Net/Channel.cpp
        Net/Epoller.h
        Net/Epoller.cpp
        Socket/Acceptor.h
        Socket/Acceptor.cpp
        Socket/InetAddress.h
        Socket/InetAddress.cpp
        Socket/Socket.h
        Socket/Socket.cpp
        Socket/SocketOps.h
        Socket/SocketOps.cpp
        Socket/TcpServer.h
        Socket/TcpServer.cpp
        Sql/SqlConnPool.h
        Sql/SqlConnPool.cpp
        Sql/SqlHandler.h
        Sql/SqlHandler.cpp
        Thread/ThreadPool.h
        Thread/ThreadObject.h
        Thread/ThreadObject.cpp
        Thread/FuncThreadPool.h
        Thread/FuncThreadPool.cpp
        Timer/Timer.h
        Timer/Timer.cpp
    )

add_library(lib_base ${base_SRCS})

find_package(Threads)

target_link_libraries(lib_base ${CMAKE_THREAD_LIBS_INIT} -lrt libmysqlclient.so)
