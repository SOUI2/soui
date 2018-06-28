#include "souistd.h"
#include <helper/slog.h>

namespace SOUI {
	//! optimze from std::stringstream to Log4zStream
#if defined (WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable:4996)
#endif


	Log4zStream::Log4zStream(char * buf, int len)
	{
		_begin = buf;
		_end = buf + len;
		_cur = _begin;
	}

	Log4zStream & Log4zStream::writeLongLong(long long t)
	{
#if defined (WIN32) || defined(_WIN64) 
		writeData("%I64d", t);
#else
		writeData("%lld", t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writeULongLong(unsigned long long t)
	{
#if defined (WIN32) || defined(_WIN64) 
		writeData("%I64u", t);
#else
		writeData("%llu", t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writePointer(const void * t)
	{
#if defined (WIN32) || defined(_WIN64)
		sizeof(t) == 8 ? writeData("%016I64x", (unsigned long long)t) : writeData("%08I64x", (unsigned long long)t);
#else
		sizeof(t) == 8 ? writeData("%016llx", (unsigned long long)t) : writeData("%08llx", (unsigned long long)t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writeBinary(const Log4zBinary & t)
	{
		writeData("%s", "\r\n\t[");
		for (int i = 0; i < t._len; i++)
		{
			if (i % 16 == 0)
			{
				writeData("%s", "\r\n\t");
				*this << (void*)(t._buf + i);
				writeData("%s", ": ");
			}
			writeData("%02x ", (unsigned char)t._buf[i]);
		}
		writeData("%s", "\r\n\t]\r\n\t");
		return *this;
	}

	Log4zStream & Log4zStream::writeString(const char* t)
	{
		writeData("%s", t);
		return *this;
	}

	Log4zStream & Log4zStream::writeWString(const wchar_t* t)
	{
#if defined (WIN32) || defined(_WIN64)
		DWORD dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
		if (dwLen < LOG4Z_LOG_BUF_SIZE)
		{
			char buf[LOG4Z_LOG_BUF_SIZE];
			dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, buf, dwLen, NULL, NULL);
			if (dwLen > 0)
			{
				buf[dwLen] = 0;
				writeData("%s", buf);
			}
		}
#else
		//not support
#endif
		return *this;
	}



#if defined (WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

}//end of namespace SOUI
