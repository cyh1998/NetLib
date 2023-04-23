//
// Created by cuiyh on 2023/4/21.
//

#ifndef NETLIB_DISPATCHER_H
#define NETLIB_DISPATCHER_H

#include <map>
#include <functional>
#include <google/protobuf/message.h>
#include "Base/Noncopyable.h"
#include "Socket/Callbacks.h"

using MessagePtr = std::shared_ptr<google::protobuf::Message>;

class Callback : Noncopyable
{
public:
    virtual ~Callback() = default;
    virtual void onMessage(const TcpConnectionPtr&, const MessagePtr&, Timestamp) const = 0;
};

template <typename T>
class CallbackT : public Callback
{
    static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T must be derived from gpb::Message.");

public:
    using ProtobufMsgTCallback = std::function<void (const TcpConnectionPtr&, const std::shared_ptr<T>&, Timestamp)>;

    explicit CallbackT(const ProtobufMsgTCallback& callback)
        : m_callback(callback)
    {
    }
    virtual ~CallbackT() = default;

    void onMessage(const TcpConnectionPtr& conn,
                   const MessagePtr& message,
                   Timestamp receiveTime) const
    {
        std::shared_ptr<T> concrete = std::static_pointer_cast<T>(message);
        if (concrete != nullptr) {
            m_callback(conn, concrete, receiveTime);
        }
    }

private:
    ProtobufMsgTCallback m_callback;
};

class ProtobufDispatcher
{
public:
    using ProtobufMsgCallback = std::function<void (const TcpConnectionPtr&, const MessagePtr&, Timestamp)>;

    explicit ProtobufDispatcher(const ProtobufMsgCallback& defaultCb)
        : m_defaultCallback(defaultCb)
    {
    }

    void onProtobufMessage(const TcpConnectionPtr& conn,
                           const MessagePtr& message,
                           Timestamp receiveTime) const
    {
        auto it = m_callbacks.find(message->GetDescriptor());
        if (it != m_callbacks.end()) {
            it->second->onMessage(conn, message, receiveTime);
        } else {
            m_defaultCallback(conn, message, receiveTime);
        }
    }

    template<typename T>
    void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageTCallback& callback)
    {
        std::shared_ptr<CallbackT<T> > pd(new CallbackT<T>(callback));
        m_callbacks[T::descriptor()] = pd;
    }

private:
    using CallbackMap = std::map<const google::protobuf::Descriptor*, std::shared_ptr<Callback> >;

    CallbackMap m_callbacks;
    ProtobufMsgCallback m_defaultCallback;
};

#endif //NETLIB_DISPATCHER_H
