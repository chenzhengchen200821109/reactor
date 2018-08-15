#ifndef TIMER_H
#define TIMER_H

#include "NonCopyable.h"

#include <atomic>
#include "Timestamp.h"
#include "Callbacks.h"

namespace muduo
{
namespace net
{
    ///
    /// Internal class for timer event.
    ///
    class Timer : public boost::noncopyable
    {
        public:
            Timer(const TimerCallback& cb, Timestamp when, double interval) : callback_(cb), expiration_(when), interval_(interval), repeat_(interval > 0.0),
                sequence_(s_numCreated_++)
            { 
            
            }

            void run() const
            {
                callback_();
            }

            Timestamp expiration() const  { return expiration_; }
            bool repeat() const { return repeat_; }
            int64_t sequence() const { return sequence_; }

            void restart(Timestamp now);

            static int64_t numCreated() { return s_numCreated_.load(); }

        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            const double interval_;
            const bool repeat_;
            const int64_t sequence_;

            // timer id
            static std::atomic<std::uint64_t> s_numCreated_;
    };

} // namespace net
} // namespace muduo
#endif  // TIMER_H
