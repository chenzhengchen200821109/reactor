#ifndef LOGFILE_H
#define LOGFILE_H

#include "FileUtil.h"
#include "Mutex.h"
#include "Types.h"

#include <memory>

namespace muduo
{
    // class LogFile -- write data to a log file
    // construct a named log file 
    class LogFile : public noncopyable
    {
        public:
            LogFile(const string& basename, off_t rollSize, bool threadSafe = true, int flushInterval = 3, int checkEveryN = 1024);
            ~LogFile();
            // append() maybe is the only interface needed for clients
            void append(const char* logline, int len);
            void flush();
            bool rollFile();

        private:
            void append_unlocked(const char* logline, int len);
            static string getLogFileName(const string& basename, time_t* now);
            const string basename_;
            const off_t rollSize_;
            const int flushInterval_;
            const int checkEveryN_;
            int count_;

            std::unique_ptr<MutexLock> mutex_;
            time_t startOfPeriod_;
            time_t lastRoll_;
            time_t lastFlush_;
            std::unique_ptr<FileUtil::AppendFile> file_; // make sure AppendFile only has one owner

            const static int kRollPerSeconds_ = 60*60*24; // roll each day
    };

} // namespace muduo
#endif // LOGFILE_H
