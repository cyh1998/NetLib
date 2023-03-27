//
// Created by cyh on 2021/3/29.
//

#include "Log.h"
#include "Thread/ThreadObject.h"

using namespace std;

Log::Log() :
    m_isOpen(false),
    m_lineCount(0),
    m_today(0),
    m_fp(nullptr),
    m_dequePtr(nullptr),
    m_logThreadPtr(nullptr)
{

}

Log::~Log() {
    if (m_logThreadPtr) {
        m_dequePtr->PushBack(m_logStr); //将未写入的缓冲区添加到队列
        while (!m_dequePtr->Empty()) m_dequePtr->Flush();
        m_dequePtr->Close(); //关闭阻塞队列
        m_logThreadPtr->join(); //join线程
    }
    if (m_fp) {
        lock_guard<mutex> locker(m_mutex);
        fclose(m_fp);
    }
}

void Log::init(const char* path, int split_lines, int max_queue_size) {
    if (m_isOpen) return;

    // 初始化
    m_dequePtr = make_unique<BlockQueue<std::string>>(max_queue_size);
    m_logThreadPtr = make_unique<ThreadObject>(FlushLogThread);
    m_logThreadPtr->start(); //启动日志线程
    m_isOpen = true;
    m_lineMax = split_lines;
    m_path = path;
    m_buf = new char[LOG_BUF_SIZE];
    memset(m_buf, '\0', LOG_BUF_SIZE);

    // 获取当前时间
    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm time = *sysTime;

    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d%02d%02d%s",
            m_path, time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, ".log");
    m_today = time.tm_mday;

    {
        lock_guard<mutex> locker(m_mutex);
        m_fp = fopen(fileName, "a");
        if (m_fp == nullptr) {
            mkdir(m_path, 0777);
            m_fp = fopen(fileName, "a");
        }
    }
}

void Log::write(int level, SourceFile file, int line, const char *format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t timer = now.tv_sec;
    struct tm *sysTime = localtime(&timer);
    struct tm time = *sysTime;
    va_list vaList;

    string logType;
    switch (level) {
        case DEBUG:
            logType = "[Debug]:";
            break;
        case INFO:
            logType = "[Info] :";
            break;
        case WARN:
            logType = "[Warn] :";
            break;
        case ERROR:
            logType = "[Error]:";
            break;
    }

    if (m_today != time.tm_mday || (m_lineCount && (m_lineCount % m_lineMax == 0))) {
        char newFile[LOG_NAME_LEN] = {0};
        char tail[16] = {0};
        snprintf(tail, 16, "%04d%02d%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);

        if (m_today != time.tm_mday) { //超过24点
            snprintf(newFile, LOG_NAME_LEN - 1, "%s/%s%s", m_path, tail, ".log");
            m_today = time.tm_mday;
            m_lineCount = 0;
        } else { //超过日志最大行数
            FlushBuff(); //同步刷新缓冲区内容到日志
            snprintf(newFile, LOG_NAME_LEN - 1, "%s/%s_%d%s", m_path, tail, (m_lineCount / m_lineMax), ".log");
        }

        lock_guard<mutex> locker(m_mutex);
        fflush(m_fp);
        fclose(m_fp);
        m_fp = fopen(newFile, "a");
    }

    {
        lock_guard<mutex> locker(m_mutex);
        ++m_lineCount;
        va_start(vaList, format);
        int n = snprintf(m_buf, 256, "%s %04d-%02d-%02d %02d:%02d:%02d.%06ld [%s:%d] ",
                logType.c_str(), time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
                time.tm_hour, time.tm_min, time.tm_sec, now.tv_usec, file.data_, line);
        int m = vsnprintf(m_buf + n, LOG_BUF_SIZE - 1, format, vaList);
        m_buf[n + m] = '\n';
        m_buf[n + m + 1] = '\0';
        m_logStr += m_buf; //将临时缓冲区内容添加到日志缓冲区
        va_end(vaList);

        if (m_logStr.size() >= LOG_BUF_SIZE) { //超过日志缓冲区大小，添加到队列
            m_dequePtr->PushBack(m_logStr);
            m_logStr = "";
        }
    }
}

void Log::flush() {
    lock_guard<mutex> locker(m_mutex);
    m_dequePtr->Flush();
    fflush(m_fp);
}

void Log::FlushLogThread() {
    g_LogMgr::instance()->AsyncWrite();
}

void Log::AsyncWrite() {
    string str;
    while (m_dequePtr->PopFront(str)) {
        lock_guard<mutex> locker(m_mutex);
        fputs(str.c_str(), m_fp);
    }
}

void Log::FlushBuff() {
    lock_guard<mutex> locker(m_mutex);
    fputs(m_logStr.c_str(), m_fp);
    m_logStr = "";
}
