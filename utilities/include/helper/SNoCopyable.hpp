#pragma once

namespace SOUI
{
	/**
	* Inheriate this class to make your class cannot be copy and assign.
	*
	*/
	class SNoCopyable
	{
	public:
		SNoCopyable() {}
		~SNoCopyable() {}
	private:
		SNoCopyable(const SNoCopyable &);
		const SNoCopyable &operator=(const SNoCopyable &);
	};

}