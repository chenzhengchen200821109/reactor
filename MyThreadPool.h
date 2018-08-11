#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "Types.h"
#include <functional>
#include "NonCopyable.h"
#include <vector>
#include <memory>
#include <deque>
#include <string>

namespace muduo
{

class ThreadPool : public noncopyable
{
    public:
        typedef std::function<void ()> Task;

        explicit ThreadPool(const string& nameArg = string("ThreadPool"), int threads = 5, int size = 5);
        ~ThreadPool();

        void setThreadInitCallback(const Task& cb) { threadInitCallback_ = cb; }
        void start();
        void stop();
        const string& name() const { return name_; }
        size_t queueSize() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.size();
        }
        // Could block if maxQueueSize > 0
        void execute(const Task& f);

    private:
        void runInThread();
        ThreadPool::Task take();
        ThreadPool::Task timeTake(double sec);
        //void start();

        int threads_;
        string name_;
        Task threadInitCallback_;
        std::vector<std::shared_ptr<muduo::Thread>> vector_;

        mutable MutexLock mutex_;
        Condition notEmpty_;
        Condition notFull_;
        Condition notFinish_;

        int size_;
        std::deque<Task> queue_;
};

} // namespace muduo

#endif
