#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"
#include "NonCopyable.h"

namespace muduo
{

class CountDownLatch : public noncopyable
{
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount() const;

    private:
        mutable MutexLock mutex_;
        Condition condition_;
        int count_;
};

} // namespace muduo
#endif  // COUNTDOWNLATCH_H
