#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "NonCopyable.h"
#include "CurrentThread.h"

class EventLoop : public muduo::noncopyable
{
    public:
        EventLoop();
        ~EventLoop();
        
        void loop();
        void assertInLoopThread()
        {
            if (!isInLoopThread()) {
                abortNotInLoopThread();
            }
        }
        bool isInLoopThread() const
        {
            return threadId_ == muduo::CurrentThread::tid();
        }

    private:
        void abortNotInLoopThread();
        bool looping_; /* atomic */
        const pid_t threadId_;

};

#endif
