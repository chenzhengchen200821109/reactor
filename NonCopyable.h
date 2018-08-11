#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace muduo
{
	//  Private copy constructor and copy assignment ensure classes derived from
	//  //  class noncopyable cannot be copied.
	//
	//  //  Contributed by Dave Abrahams
	class noncopyable
	{
		public:
			noncopyable() {}
			~noncopyable() {}
			noncopyable(const noncopyable& ) = delete; /* since c++11 */
			noncopyable& operator=(const noncopyable& ) = delete; /* since c++11 */
	};

} // namespace muduo


#endif
