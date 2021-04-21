set(base_SRCS
    Base/Noncopyable.h
    Log/BlockQueue.h
    Log/Log.h
    Log/Log.cpp
    Thread/ThreadPool.h
    Thread/ThreadObject.h
    Thread/ThreadObject.cpp
    Thread/FuncThreadPool.h
    Thread/FuncThreadPool.cpp
    Timer/TimerHeap.h
    Timer/TimerHeap.cpp
    Timer/TimerList.h
    Timer/TimerList.cpp
    Timer/TimerWheel.h
    Timer/TimerWheel.cpp
    )

add_library(lib_base ${base_SRCS})

find_package(Threads)

target_link_libraries(lib_base ${CMAKE_THREAD_LIBS_INIT} -lrt)
