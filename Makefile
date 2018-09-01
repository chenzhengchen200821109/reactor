logging_test:
	g++ -g -std=c++11 Logging_test.cc Logging.cc LogFile.cc Condition.cc ThreadPool.cc FileUtil.cc LogStream.cc Date.cc Timestamp.cc Thread.cc TimeZone.cc CountDownLatch.cc ProcessInfo.cc -pthread
asynclogging_test:
	g++ -g -std=c++11 AsyncLogging_test.cc AsyncLogging.cc Logging.cc LogFile.cc Condition.cc ThreadPool.cc FileUtil.cc LogStream.cc Date.cc Timestamp.cc Thread.cc TimeZone.cc CountDownLatch.cc ProcessInfo.cc -pthread
EventLoop_test:
	g++ -g -std=c++11 EventLoop_test.cc EventLoop.cc Channel.cc SocketsOps.cc AsyncLogging.cc Logging.cc LogFile.cc Condition.cc ThreadPool.cc FileUtil.cc LogStream.cc Date.cc Timestamp.cc Thread.cc TimeZone.cc CountDownLatch.cc ProcessInfo.cc -pthread
clean:
	rm -rf a.out
