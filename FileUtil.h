#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "StringPiece.h"
#include "NonCopyable.h"
#include <sys/types.h>  // for off_t

namespace muduo
{

namespace FileUtil
{
    // class ReadSmallFile deal with reading from a file.
    // class AppendFile deal with writing to a file.

	// read small file < 64KB
	class ReadSmallFile : public noncopyable
	{
 		public:
  			ReadSmallFile(StringArg filename);
  			~ReadSmallFile();

  			// return errno
  			template<typename String>
  			int readToString(int maxSize, String* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime);

  			// Read at maxium kBufferSize into buf_
  			// return errno
  			int readToBuffer(int* size);

  			const char* buffer() const { return buf_; }

  			static const int kBufferSize = 64*1024;

 		private:
  			int fd_;
  			int err_;
  			char buf_[kBufferSize];
	};

	// template function
	// read the file content, returns errno if error happens.
	template<typename String>
	int readFile(StringArg filename, int maxSize, String* content, int64_t* fileSize = NULL, int64_t* modifyTime = NULL, int64_t* createTime = NULL)
	{
  		ReadSmallFile file(filename);
  		return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
	}

	// not thread safe
    // File I/O handle
	class AppendFile : public noncopyable
	{
 		public:
  			explicit AppendFile(StringArg filename);
			~AppendFile();

            // public interface for clients. we get data from logline and write to a file.
  			void append(const char* logline, const size_t len);
  			void flush();

  			off_t writtenBytes() const { return writtenBytes_; }

 		private:
  			size_t write(const char* logline, size_t len);

  			FILE* fp_;
  			char buffer_[64*1024];
  			off_t writtenBytes_;
	};

} // namespace FileUtil

} // namespace muduo

#endif  // FILEUTIL_H

