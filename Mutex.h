#ifndef MUTEX_H
#define MUTEX_H

#include "NonCopyable.h"
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#ifdef CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum;})

#endif // CHECK_PTHREAD_RETURN_VALUE

namespace muduo
{

    // Use as data member of a class, eg.
    //
    // class Foo
    // {
    //  public:
    //   int size() const;
    //
    //  private:
    //   mutable MutexLock mutex_;
    //   std::vector<int> data_; // GUARDED BY mutex_
    // };
    class MutexLock : public noncopyable
    {
        public:
            MutexLock() : holder_(0)
            {
                MCHECK(pthread_mutex_init(&mutex_, NULL));
            }

            ~MutexLock()
            {
                assert(holder_ == 0);
                MCHECK(pthread_mutex_destroy(&mutex_));
            }

            // must be called when locked, i.e. for assertion
            bool isLockedByThisThread() const
            {
                //return holder_ == CurrentThread::tid();
                return holder_ == static_cast<pid_t>(::syscall(SYS_gettid));
            }

            void assertLocked() const
            {
                assert(isLockedByThisThread());
            }

            // internal usage

            void lock()
            {
                MCHECK(pthread_mutex_lock(&mutex_));
                assignHolder();
            }

            void unlock()
            {
                unassignHolder();
                MCHECK(pthread_mutex_unlock(&mutex_));
            }

            pthread_mutex_t* getPthreadMutex() /* non-const */
            {
                return &mutex_;
            }

        private:
            friend class Condition;
            //?????????????????????????????????
            class UnassignGuard : noncopyable
            {
                public:
                    UnassignGuard(MutexLock& owner) : owner_(owner)
                    {
                        owner_.unassignHolder();
                    }

                    ~UnassignGuard()
                    {
                        owner_.assignHolder();
                    }

                private:
                    MutexLock& owner_;
            };

            void unassignHolder()
            {
                holder_ = 0;
            }

            void assignHolder()
            {
                //holder_ = CurrentThread::tid();
                holder_ = static_cast<pid_t>(::syscall(SYS_gettid));
            }

            pthread_mutex_t mutex_;
            pid_t holder_;
    };

    // Use as a stack variable, eg.
    // int Foo::size() const
    // {
    //   MutexLockGuard lock(mutex_);
    //   return data_.size();
    // }
    class MutexLockGuard : noncopyable
    {
        public:
            explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex)
            {
                mutex_.lock();
            }

            ~MutexLockGuard()
            {
                mutex_.unlock();
            }

        private:
            MutexLock& mutex_;
    };

} // namespace muduo

// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

#endif // MUTEX_H
