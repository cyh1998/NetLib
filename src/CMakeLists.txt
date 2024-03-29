include_directories(PRIVATE ${PROJECT_SOURCE_DIR}/src)
include_directories(Protobuf ${Protobuf_INCLUDE_DIR})

set(base_SRCS
        Base/Condition.h
        Base/MutexLock.h
        Base/Noncopyable.h
        Base/Singleton.h
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
        Protobuf/Codec.h
        Protobuf/Codec.cpp
        Protobuf/Dispatcher.h
        Protobuf/google-inl.h
        Socket/Acceptor.h
        Socket/Acceptor.cpp
        Socket/Buffer.h
        Socket/Buffer.cpp
        Socket/Callbacks.h
        Socket/Endian.h
        Socket/InetAddress.h
        Socket/InetAddress.cpp
        Socket/Socket.h
        Socket/Socket.cpp
        Socket/SocketOps.h
        Socket/SocketOps.cpp
        Socket/TcpConnection.cpp
        Socket/TcpConnection.h
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
        Timer/TimerQueue.h
        Timer/TimerQueue.cpp
        )

add_library(base ${base_SRCS})

target_link_libraries(base ${Protobuf_LIBRARIES} pthread -lrt libmysqlclient.so)
