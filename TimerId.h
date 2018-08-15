#ifndef TIMERID_H
#define TIMERID_H

#include "Copyable.h"
#include <memory>

namespace muduo
{
namespace net
{

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId : public muduo::copyable
{
    public:
        TimerId() : timer_(NULL), sequence_(0)
        {

        }

        //TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq)
        TimerId(std::unique_ptr<Timer> timer, int64_t seq) : timer_(std::move(timer)), sequence_(seq)
        {

        }

        // default copy-ctor, dtor and assignment are okay

    friend class TimerQueue;

    private:
        //Timer* timer_;
        std::unique_ptr<Timer> timer_;
        int64_t sequence_;
};

} // namespace net
} // namespace muduo

#endif  // TIMERID_H
