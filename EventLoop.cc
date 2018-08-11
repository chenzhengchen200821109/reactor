#include "EventLoop.h"
#include "Logging.h"

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop() : looping_(false), threadId_(muduo::CurrentThread::tid())
{
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }

}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}
