#ifndef TIMERID_H
#define TIMERID_H

#include "Copyable.h"
//#include "NonCopyable.h"
#include <memory>
#include "Timer.h"

namespace muduo
{
namespace net
{

//class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId : public muduo::copyable
//class TimerId : public noncopyable
{
    public:
        TimerId() : /* timer_(NULL),*/ sequence_(0)
        {

        }

        //TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq)
        TimerId(std::unique_ptr<Timer> timer, int64_t seq) : timer_(std::move(timer)), sequence_(seq)
        {

        }

        //TimerId(const TimerId& timerId) : timer_(std::move(timerId.timer_), sequence_(std::move(timerId.sequence_))
        //{
            //timer_ = std::move(timerId.timer_);
        //}
        // default copy-ctor, dtor and assignment are okay
        // TODO: implement copy-ctor, dtor and assignment

    friend class TimerQueue;

    private:
        //Timer* timer_;
        std::unique_ptr<Timer> timer_;
        int64_t sequence_;
};

} // namespace net
} // namespace muduo

#endif  // TIMERID_H
