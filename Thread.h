#ifndef THREAD_H
#define THREAD_H

#include <atomic>
#include "CountDownLatch.h"
#include "Types.h"
#include <functional>
#include "NonCopyable.h"
#include <memory>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <string>

namespace muduo
{

class Thread : public noncopyable
{
    public:
        typedef std::function<void ()> ThreadFunc;

        explicit Thread(const ThreadFunc&, const string& name = string());
        ~Thread();

        void start();
        int join(); // return pthread_join()
        bool started() const { return started_; }
        // pthread_t pthreadId() const { return pthreadId_; }
        pid_t tid() const { return tid_; }
        string tidToString() 
        {
            char tid[32];
            snprintf(tid, sizeof tid, "%5d", tid_);
            return tid;
        }
        bool isMainThread()
        {
            return tid() == ::getpid();
        }
        const string& name() const { return name_; }
        static int numCreated() { return numCreated_.load(); }

    private:
        void setDefaultName();
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        string name_;
        CountDownLatch latch_; // always assure working thread run first
        static std::atomic<int> numCreated_;
};

}
#endif
