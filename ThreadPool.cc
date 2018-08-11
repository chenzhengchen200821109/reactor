#include "ThreadPool.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <functional>
//#include <stdio.h>

using namespace muduo;
using namespace std::placeholders;

ThreadPool::ThreadPool(const string& nameArg, int threads, int size) : name_(nameArg), queue_(0), threads_(threads), size_(size), mutex_(), notEmpty_(mutex_), notFull_(mutex_) ,notFinish_(mutex_), running_(false)
{
    //start();
}

ThreadPool::~ThreadPool()
{
    if (running_)
        stop();
}

void ThreadPool::start()
{
    running_ = true;
    vector_.reserve(threads_);
    for(int i = 0; i < threads_; ++i) {
        char id[32];
        snprintf(id, sizeof id, "%d", i+1);
        std::shared_ptr<muduo::Thread> elem(new muduo::Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
        vector_.push_back(elem);
        vector_[i]->start();
    }
    if (threadInitCallback_) {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{

    running_ = false;
    {
        MutexLockGuard lock(mutex_);
        while (threads_ > 0) {
            notFinish_.wait();
            printf("wait for a thread\n");
        }
    }
    std::for_each(vector_.begin(), vector_.end(), std::bind(&muduo::Thread::join, _1));
}

void ThreadPool::execute(const Task& task)
{
    MutexLockGuard lock(mutex_);
    while (queue_.size() == size_) {
        notFull_.wait();
    }
    queue_.push_back(task);
    //printf("put a value\n");
    notEmpty_.notify();
}

ThreadPool::Task ThreadPool::take()
{
    while (queue_.empty()) {
        notEmpty_.wait();
    }
    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        notFull_.notify();
    }
    return task;
}

ThreadPool::Task ThreadPool::timeTake(double sec)
{
    while (queue_.empty()) {
        if (notEmpty_.timeWait(sec)) {
            threads_--;
            notFinish_.notify();
            break;
        }
    }
    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        //printf("take a task");
        notFull_.notify();
    }
    return task;
}

void ThreadPool::runInThread()
{
    try {
        while (1) {
            mutex_.lock();
            Task task;
            task = timeTake(2);
            if (task) {
                task();
            }
            else {
                mutex_.unlock();
                break;
            }
            mutex_.unlock();
        }
    }
    //catch (const Exception& ex) {
    //    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    //    fprintf(stderr, "reason: %s\n", ex.what());
    //    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    //    abort();
    //}
    catch (const std::exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...) {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}

