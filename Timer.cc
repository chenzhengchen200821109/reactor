#include "Timer.h"

using namespace muduo;
using namespace muduo::net;

std::atomic<std::uint64_t> Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
    if (repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = Timestamp::invalid(); // construct an invalid Timestamp
    }
}
