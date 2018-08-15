#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <functional>
#include <memory>
#include "Timestamp.h"

namespace muduo
{

// Adapted from google-protobuf stubs/common.h
// see License in muduo/base/Types.h
template<typename To, typename From>
inline std::shared_ptr<To> down_pointer_cast(const std::shared_ptr<From>& f)
{
    if (false) {
        implicit_cast<From*, To*>(0);
    }
    return std::static_pointer_cast<To>(f);
}

namespace net
{

// All client visible callbacks go here.

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

// the data has been read to (buf, len)
typedef std::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
                            Buffer* buffer,
                            Timestamp receiveTime);

} // namespace net
} // namespace muduo

#endif  // CALLBACKS_H
