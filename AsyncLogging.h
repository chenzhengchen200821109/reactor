#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "BlockingQueue.h"
#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"
#include <boost/bind.hpp>
#include "NonCopyable.h"
#include <vector>
#include <memory>

namespace muduo
{

    class AsyncLogging : public noncopyable
    {
        public:
            AsyncLogging(const string& basename, off_t rollSize, int flushInterval = 3);
            ~AsyncLogging()
            {
                if (running_) {
                    stop();
                }
            }

            void append(const char* logline, int len);

            void start()
            {
                running_ = true;
                thread_.start();
                latch_.wait();
            }

            void stop()
            {
                running_ = false;
                cond_.notify();
        //BufferPtr nextBuffer_;
                thread_.join();
            }

        private:

        // declare but not define, prevent compiler-synthesized functions
        AsyncLogging(const AsyncLogging&) = delete;
        void operator=(const AsyncLogging&) = delete;

        void threadFunc();

        typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer> > BufferVector;

        const int flushInterval_;
        bool running_;
        string basename_;
        off_t rollSize_;
        muduo::Thread thread_;
        muduo::CountDownLatch latch_;
        muduo::MutexLock mutex_;
        muduo::Condition cond_;
        std::unique_ptr<Buffer> currentBuffer_;
        std::unique_ptr<Buffer> nextBuffer_;
        BufferVector buffers_;
    };

} // namespace muduo

#endif  // ASYNCLOGGING_H
