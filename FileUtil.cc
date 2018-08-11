#include "FileUtil.h"
//#include "Logging.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace muduo;

FileUtil::AppendFile::AppendFile(StringArg filename) : fp_(::fopen(filename.c_str(), "ae")), writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof buffer_); // set up buffer for a file stream
}

FileUtil::AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void FileUtil::AppendFile::append(const char* logline, const size_t len)
{
    size_t n = write(logline, len);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err)); // strerror_tl()
            }
            break;
        }
        n += x;
        remain = len - n; // remain -= x
    }
    writtenBytes_ += len;
}

void FileUtil::AppendFile::flush()
{
    ::fflush(fp_); // flush a stream
}

// write is a private member function
size_t FileUtil::AppendFile::write(const char* logline, size_t len)
{
    /*
     * The stdio functions are thread-safe. This is achieved by assigning to each
     * FILE object a lockcount and (if the lockcount is nonzero) and owning thread.
     */
    return ::fwrite_unlocked(logline, 1, len, fp_); // fwrite_unlocked() is similar to fwrite()
  												  // except it is not thread-safe
}

FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename) : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), err_(0)
{
    buf_[0] = '\0';
    if (fd_ < 0) {
        err_ = errno;
    }
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
    if (fd_ >= 0) {
        ::close(fd_); // FIXME: check EINTR
    }
}

// read maxSize bytes from a file and write to destination pointed by content
// return errno
template<typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize, String* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime)
{
    static_assert(sizeof(off_t) >= 4, "type off_t is too small");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0) {
        content->clear();
        if (fileSize) {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
                }
                else if (S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }
                if (modifyTime) {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime) {
                    *createTime = statbuf.st_ctime;
                }
            } 
            else {
                err = errno;
            }
        }

        while (content->size() < static_cast<size_t>(maxSize)) {
            size_t toRead = std::min(static_cast<size_t>(maxSize) - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0) {
                content->append(buf_, n);
            }
            else {
                if (n < 0) {
                    err = errno;
                }
                break; // an error occurs
            }
        }
    }
    return err;
}

// read from a file and write to the internal buffer
int FileUtil::ReadSmallFile::readToBuffer(int* size)
{
    int err = err_;
    if (fd_ >= 0) {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0); // pread() performs the same function
        if (n >= 0) {									   // position in the file without modifying
            if (size) {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        }
        else {
            err = errno;
        }
    }
    return err;
}

// instantiation
template int FileUtil::readFile(StringArg filename, int maxSize, string* content, int64_t*, int64_t*, int64_t*);
template int FileUtil::ReadSmallFile::readToString(int maxSize, string* content, int64_t*, int64_t*, int64_t*);

