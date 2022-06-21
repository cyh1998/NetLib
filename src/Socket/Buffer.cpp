//
// Created by cyh on 2022/6/17.
//

#include <sys/uio.h>
#include "Buffer.h"
#include "SocketOps.h"

ssize_t Buffer::readFd(int fd, int *savedErrno) {
    // 申请栈上空间
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();

    // 两块iovec分别指向内部buffer的可写空间和栈上空间
    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    // 判断内部buffer的可写空间是否足够
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0) {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        // 内部空间足够，直接写入，移动可写索引
        m_writerIndex += n;
    } else {
        // 内部空间不足，先写入栈上空间，再将栈上数据append到内部空间
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}
