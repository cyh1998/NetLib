//
// Created by cyh on 2022/6/17.
//

#ifndef NETLIB_BUFFER_H
#define NETLIB_BUFFER_H

#include <assert.h>
#include <vector>
#include <string>

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize) :
        m_buffer(kCheapPrepend + initialSize),
        m_readerIndex(kCheapPrepend),
        m_writerIndex(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    // 交换两个Buffer
    void swap(Buffer& rhs)
    {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }

    // 可读字节数
    size_t readableBytes() const
    { return m_writerIndex - m_readerIndex; }

    // 可写字节数
    size_t writableBytes() const
    { return m_buffer.size() - m_writerIndex; }

    // 可读数据索引
    size_t prependableBytes() const
    { return m_readerIndex; }

    // 可读数据起始地址
    const char* peek() const
    { return begin() + m_readerIndex; }

    // 回收len字节Buffer
    // 从缓冲区读走字节后的处理，移动可读索引
    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            m_readerIndex += len;
        } else {
            retrieveAll();
        }
    }

    // 回收全部Buffer
    // 从缓冲区读走全部字节，直接重置读写索引
    void retrieveAll()
    {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    // 读取len字节为string
    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    // 读取全部字节为string
    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    // 写入数据
    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }
    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    // 往缓冲区写入数据后操作，移动可写索引
    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        m_writerIndex += len;
    }

    // 确认可写空间是否足够
    void ensureWritableBytes(size_t len)
    {
        // 可写空间不足
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    // 可写数据起始地址
    char* beginWrite()
    { return begin() + m_writerIndex; }
    const char* beginWrite() const
    { return begin() + m_writerIndex; }

    // 前部写入数据
    void prepend(const void* data, size_t len)
    {
        assert(len <= prependableBytes());
        m_readerIndex -= len; //先前移可写索引
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + m_readerIndex);
    }

    size_t internalCapacity() const {
        return m_buffer.capacity();
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    // 缓存区首地址
    // 可以用vector::data()
    char* begin()
    { return &*m_buffer.begin(); }
    const char* begin() const
    { return &*m_buffer.begin(); }

    // 内部腾挪或拓展空间
    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            // 内部空间不足，通过resize()拓展空间
            m_buffer.resize(m_writerIndex+len);
        } else {
            // 可读数据索引前还有足够空间，将可读数据前移
            assert(kCheapPrepend < m_readerIndex);
            size_t readable = readableBytes();
            std::copy(begin() + m_readerIndex,
                      begin() + m_writerIndex,
                      begin() + kCheapPrepend);
            // 修改读写索引
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> m_buffer; //内部存储
    size_t m_readerIndex; //可读索引
    size_t m_writerIndex; //可写索引
};

#endif //NETLIB_BUFFER_H
