//
// Created by cuiyh on 2023/4/11.
//

#include <unordered_map>
#include <zlib.h>  //adler32
#include "Log/Log.h"
#include "Socket/Endian.h"
#include "google-inl.h"
#include "Codec.h"

namespace
{
    const std::unordered_map<ProtobufCodec::ErrorCode, std::string> errCodeString {
        {ProtobufCodec::kNoError, "NoError"},
        {ProtobufCodec::kInvalidLength, "InvalidLength"},
        {ProtobufCodec::kCheckSumError, "CheckSumError"},
        {ProtobufCodec::kInvalidNameLen, "InvalidNameLen"},
        {ProtobufCodec::kUnknownMessageType, "UnknownMessageType"},
        {ProtobufCodec::kParseError, "ParseError"}
     };
    const std::string kUnknownErrorStr = "UnknownError";
}

const std::string &ProtobufCodec::errorCodeToString(ProtobufCodec::ErrorCode errorCode)
{
    if (errCodeString.find(errorCode) != errCodeString.end()) {
        return errCodeString.at(errorCode);
    } else {
        return kUnknownErrorStr;
    }
}

void ProtobufCodec::fillEmptyBuffer(Buffer *buf, const google::protobuf::Message &message)
{
    if (buf->readableBytes() != 0) {
        return;
    }

    const std::string& typeName = message.GetTypeName();
    int32_t nameLen = static_cast<int32_t>(typeName.size() + 1);
    int32_t net32 = hostToNetwork32(nameLen);
    buf->append(&net32, sizeof(net32));
    buf->append(typeName.c_str(), nameLen);

    // code copied from MessageLite::SerializeToArray() and MessageLite::SerializePartialToArray().
    GOOGLE_DCHECK(message.IsInitialized()) << InitializationErrorMessage("serialize", message);

    int byte_size = google::protobuf::internal::ToIntSize(message.ByteSizeLong());
    buf->ensureWritableBytes(byte_size);

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->beginWrite());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);
    if (end - start != byte_size) {
        ByteSizeConsistencyError(byte_size,
                                 google::protobuf::internal::ToIntSize(message.ByteSizeLong()),
                                 static_cast<int>(end - start));
    }
    buf->hasWritten(byte_size);

    int32_t checkSum = static_cast<int32_t>(::adler32(1,
                                            reinterpret_cast<const Bytef*>(buf->peek()),
                                            static_cast<int>(buf->readableBytes())));
    net32 = hostToNetwork32(checkSum);
    buf->append(&net32, sizeof(net32));
    if(buf->readableBytes() != sizeof(nameLen) + nameLen + byte_size + sizeof(checkSum)) {
        buf->retrieveAll();
        return;
    }

    int32_t len = hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
    buf->prepend(&len, sizeof len);
}

google::protobuf::Message *ProtobufCodec::createMessage(const std::string &typeName)
{
    google::protobuf::Message* message = nullptr;
    const google::protobuf::Descriptor* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (descriptor)
    {
        const google::protobuf::Message* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}

MessagePtr ProtobufCodec::parse(const char *buf, int len, ProtobufCodec::ErrorCode *errorCode)
{
    MessagePtr message;

    int32_t expectedCheckSum = asInt32(buf + len - kHeaderLen);
    int32_t checkSum = static_cast<int32_t>(::adler32(1,
                                                      reinterpret_cast<const Bytef*>(buf),
                                                      static_cast<int>(len - kHeaderLen)));
    if (checkSum == expectedCheckSum)
    {
        int32_t nameLen = asInt32(buf);
        if (nameLen >= 2 && nameLen <= len - 2 * kHeaderLen)
        {
            std::string typeName(buf + kHeaderLen, buf + kHeaderLen + nameLen - 1);
            message.reset(createMessage(typeName));
            if (message)
            {
                const char* data = buf + kHeaderLen + nameLen;
                int32_t dataLen = len - nameLen - 2 * kHeaderLen;
                if (message->ParseFromArray(data, dataLen)) {
                    *errorCode = kNoError;
                } else {
                    *errorCode = kParseError;
                }
            } else {
                *errorCode = kUnknownMessageType;
            }
        } else {
            *errorCode = kInvalidNameLen;
        }
    } else {
        *errorCode = kCheckSumError;
    }

    return message;
}

void ProtobufCodec::defaultErrorCallback(const TcpConnectionPtr &conn,
                                         Buffer *buf,
                                         Timestamp,
                                         ProtobufCodec::ErrorCode errorCode)
{
    LOG_ERROR("ProtobufCodec::defaultErrorCallback - %s", errorCodeToString(errorCode).c_str());
    if (conn && conn->connected()) {
        conn->shutdown();
    }
}

int32_t ProtobufCodec::asInt32(const char *buf)
{
    int32_t be32 = 0;
    ::memcpy(&be32, buf, sizeof(be32));
    return networkToHost32(be32);
}

void ProtobufCodec::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime) {

}

void ProtobufCodec::send(const TcpConnectionPtr &conn, const google::protobuf::Message &message)
{
    Buffer buf;
    fillEmptyBuffer(&buf, message);
    conn->send(std::move(buf));
}



