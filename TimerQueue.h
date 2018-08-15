#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <vector>
#include <memory>
#include <atomic>

#include "NonCopyable.h"

#include "Mutex.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "Channel.h"

namespace muduo
{
namespace net
{

    class EventLoop;
    class Timer;
    class TimerId;

    ///
    /// A best efforts timer queue.
    /// No guarantee that the callback will be on time.
    ///
    class TimerQueue : public noncopyable
    {
        public:
            explicit TimerQueue(EventLoop* loop);
            ~TimerQueue();

            ///
            /// Schedules the callback to be run at given time,
            /// repeats if @c interval > 0.0.
            ///
            /// Must be thread safe. Usually be called from other threads.
            TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

            void cancel(TimerId timerId);

        private:

            // FIXME: use unique_ptr<Timer> instead of raw pointers.
            // This requires heterogeneous comparison lookup (N3465) from C++14
            // so that we can find an T* in a set<unique_ptr<T>>.
            //typedef std::pair<Timestamp, Timer*> Entry;
            typedef std::pair<Timestamp, std::unique_ptr<Timer>> Entry;
            typedef std::set<Entry> TimerList;
            //typedef std::pair<Timer*, int64_t> ActiveTimer;
            typedef std::pair<std::unique<Timer>, int64_t> ActiveTimer;
            typedef std::set<ActiveTimer> ActiveTimerSet;

            //void addTimerInLoop(Timer* timer);
            void addTimerInLoop(std::unique_ptr<Timer> timer);
            void cancelInLoop(TimerId timerId);
            // called when timerfd alarms
            void handleRead();
            // move out all expired timers
            std::vector<Entry> getExpired(Timestamp now);
            void reset(const std::vector<Entry>& expired, Timestamp now);

            //bool insert(Timer* timer);
            bool insert(std::unique_ptr<Timer> timer);

            EventLoop* loop_;
            const int timerfd_;
            Channel timerfdChannel_;
            // Timer list sorted by expiration
            TimerList timers_;

            // for cancel()
            ActiveTimerSet activeTimers_;
            //bool callingExpiredTimers_; /* atomic */
            std::atomic<bool> callingExpiredTimers_;
            ActiveTimerSet cancelingTimers_;
    };

} // namespace net
} // namespace muduo
#endif  // TIMERQUEUE_H
