//
// Created by cyh on 2021/3/29.
//

#ifndef NETLIB_LOG_H
#define NETLIB_LOG_H

#include <iostream>
#include <mutex>
#include <cstdio> //fopen fputs fflush
#include <string>
#include <cstring> //memset
#include <cstdarg> //va_start va_end
#include <sys/stat.h> //mkdir
#include <sys/time.h> //gettimeofday
#include "BlockQueue.h"
#include "Singleton.h"

class ThreadObject;

class Log {
public:
    enum {
        DEBUG,
        INFO,
        WARN,
        ERROR,
    };

    class SourceFile {
    public:
        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = strrchr(filename, '/');
            if (slash) data_ = slash + 1;
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Log();
    virtual ~Log();

    void init(const char* path = "./log/", int split_lines = 5000000, int max_queue_size = 1000);
    static void FlushLogThread();

    void write(int level, SourceFile file, int line, const char *format, ...); 
    void flush();
    bool isOpen() const { return m_isOpen; }

private:
    void AsyncWrite();
    void FlushBuff();

private:
    static const int LOG_NAME_LEN = 256; //日志文件名长度
    static const int LOG_BUF_SIZE = 8192; //日志缓冲区大小

    bool m_isOpen; //是否开启日志
    int m_lineMax; //日志最大行数
    int m_lineCount; //已写日志行数
    const char *m_path; //日志文件路径
    int m_today; //当前时间的天
    char *m_buf; //临时日志缓冲区
    std::string m_logStr; //日志缓冲区

    FILE* m_fp;
    std::unique_ptr<BlockQueue<std::string>> m_dequePtr;
    std::unique_ptr<ThreadObject> m_logThreadPtr;
    std::mutex m_mutex;

};

using g_LogMgr = Singleton<Log>;

#define LOG_BASE(level, format, ...) \
    do { \
        Log* log = g_LogMgr::instance(); \
        if (log->isOpen()){ \
            log->write(level, Log::SourceFile(__FILE__), __LINE__, format, ##__VA_ARGS__); \
	    log->flush(); \
        } \
    } while(false);

#define LOG_DEBUG(format, ...) LOG_BASE(Log::DEBUG, format, ##__VA_ARGS__);
#define LOG_INFO(format, ...)  LOG_BASE(Log::INFO, format, ##__VA_ARGS__);
#define LOG_WARN(format, ...)  LOG_BASE(Log::WARN, format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) LOG_BASE(Log::ERROR, format, ##__VA_ARGS__);

#endif //NETLIB_LOG_H
