//
// Created by cuiyh on 2023/4/11.
//

#ifndef NETLIB_CODEC_H
#define NETLIB_CODEC_H

#include <string>
#include <memory>
#include <google/protobuf/message.h>
#include "Base/Noncopyable.h"
#include "Socket/Buffer.h"
#include "Socket/TcpConnection.h"

using MessagePtr = std::shared_ptr<google::protobuf::Message>;

class ProtobufCodec : Noncopyable
{
public:
    enum ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    // 接收回调函数别名
    using ProtobufMessageCallback = std::function<void (const TcpConnectionPtr &, const MessagePtr &, Timestamp)>;

    // 错误回调函数别名
    using ErrorCallback = std::function<void (const TcpConnectionPtr &, Buffer *, Timestamp, ErrorCode)>;

    explicit ProtobufCodec(const ProtobufMessageCallback &messageCb)
        : m_messageCallback(messageCb),
          m_errorCallback(defaultErrorCallback)
    {
    }

    ProtobufCodec(const ProtobufMessageCallback &messageCb, const ErrorCallback &errorCb)
        : m_messageCallback(messageCb),
          m_errorCallback(errorCb)
    {
    }

public:
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
    void send(const TcpConnectionPtr& conn, const google::protobuf::Message& message);

    static const std::string& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(Buffer *buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& typeName);
    static MessagePtr parse(const char *buf, int len, ErrorCode *errorCode);

private:
    // 默认错误回调函数
    static void defaultErrorCallback(const TcpConnectionPtr &conn, Buffer *buf, Timestamp, ErrorCode errorCode);

    static int32_t asInt32(const char* buf);

private:
    ProtobufMessageCallback m_messageCallback; //接收回调函数
    ErrorCallback m_errorCallback; //错误回调函数

    const static int kHeaderLen = sizeof(int32_t);
    const static int kMinMessageLen = 2 * kHeaderLen + 2;
    const static int kMaxMessageLen = 64 * 1024 * 1024;
};

#endif //NETLIB_CODEC_H
