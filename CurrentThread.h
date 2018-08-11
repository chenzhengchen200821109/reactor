#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include "Timestamp.h"

namespace muduo
{
    //class CurrentThread : public noncopyable
    //{
    //    public:
    //        static int tid;
    //        static char tidString[32];
    //        static int tidStringLength;
    //        static const char* threadName;
    //        static int tid() const { return (tid = gettid()); }
    //        static string tidToString() const 
    //        { 
    //            tidString = snprintf(tidString, sizeof tidString, "%5d", tid);
    //            return tidString;
    //        }
    //        static bool isMainThread() const 
    //        { 
    //            return tid() == ::getpid();
    //        }
    //        static const char* name() { return threadName; }
    //        static void sleepUsec(int64_t usec)
    //        {
    //            struct timespec ts = { 0, 0 };
    //            ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    //            ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    //            ::nanosleep(&ts, NULL);
    //        }
    //    private:
    //        CurrentThread() {}
    //        ~CurrentThread() {}
    //        static pid_t gettid()
    //        {
    //            return static_cast<pid_t>(::syscall(SYS_gettid));
    //        }
    //};
namespace CurrentThread
{
// internal
//    extern __thread int t_cachedTid;
//    extern __thread char t_tidString[32];
//    extern __thread int t_tidStringLength;
//    extern __thread const char* t_threadName;
//    void cacheTid();
    static int tid_ = 0;
    static char tidString_[32] = { '\0', '\0', '\0' };
    static int tidStringLength_ = 6;
    static const char* threadName_ = "unknown";

    inline pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    inline int tid() 
    {
        //if (__builtin_expect(t_cachedTid == 0, 0)) {
        //    cacheTid();
        //}
        //return t_cachedTid;
        //if (tid_ == 0) {
        //    tid_ = gettid();
        //}
        //return tid_;
        return (tid_ = gettid());
    }

    inline std::string tidToString() 
    { 
        tidStringLength_ = snprintf(tidString_, sizeof tidString_, "%5d", tid_);
        return tidString_;
    }
    inline const char* tidString() 
    {
        tidToString();
        return tidString_;
    }

    inline int tidStringLength() // for logging
    {
        //return t_tidStringLength;
        tidToString();
        return tidStringLength_;
    }

    inline const char* name()
    {
        //return t_threadName;
        return threadName_;
    }

    inline bool isMainThread()
    {
        return tid() == ::getpid();
    }

    inline void sleepUsec(int64_t usec) 
    {
        struct timespec ts = { 0, 0 };
        ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
    }
    
} // namespace CurrentThread

} // namespace muduo

#endif
