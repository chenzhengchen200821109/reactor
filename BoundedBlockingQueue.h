#ifndef BOUNDEDBLOCKINGQUEUE_H
#define BOUNDEDBLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"
#include "NonCopyable.h"
#include <assert.h>
#include <deque>
//#include <stdio.h>

namespace muduo
{

    template<typename T>
    class BoundedBlockingQueue : public noncopyable
    {
        public:
        explicit BoundedBlockingQueue(int maxSize) : mutex_(), notEmpty_(mutex_), notFull_(mutex_), queue_(0), maxSize_(maxSize)
        {

        }

        void put(const T& x)
        {
            MutexLockGuard lock(mutex_);
            while (queue_.size() == maxSize_) {
                notFull_.wait();
            }
            assert(queue_.size() <= maxSize_);
            queue_.push_back(x);
            //printf("put a value\n");
            notEmpty_.notify();
        }

        T take()
        {
            MutexLockGuard lock(mutex_);
            while (queue_.empty()) {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            /* calling front() on an empty container causes undefined behavior */
            T front(queue_.front());
            queue_.pop_front();
            //printf("take a value\n");
            notFull_.notify();
            return front;
        }

        T timeTake(double sec)
        {
            MutexLockGuard lock(mutex_);
            while (queue_.empty()) {
                notEmpty_.timeWait(sec);
            }
            /* 
             * the following happens because no more data put into queue and we 
             * don't want to wait.
             */
            T t;
            if (!queue_.empty()) {
                t = queue_.front();
                queue_.pop_front();
                notFull_.notify();
            }
            return t;
        }

        bool empty() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.empty();
        }

        bool full() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.size() == maxSize_;
        }

        size_t size() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.size();
        }

        size_t capacity() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.max_size();
        }

    private:
        mutable MutexLock mutex_;
        Condition notEmpty_;
        Condition notFull_;
        int maxSize_;
        std::deque<T>  queue_;
    };

}

#endif  // BOUNDEDBLOCKINGQUEUE_H
