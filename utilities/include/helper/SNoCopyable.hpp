#pragma once
#include <utilities-def.h>
namespace SOUI
{
	/**
	* Inheriate this class to make your class cannot be copy and assign.
	*
	*/
	class UTILITIES_API SNoCopyable
	{
	public:
		SNoCopyable() {}
		~SNoCopyable() {}
	private:
		SNoCopyable(const SNoCopyable &);
		const SNoCopyable &operator=(const SNoCopyable &);
	};

}