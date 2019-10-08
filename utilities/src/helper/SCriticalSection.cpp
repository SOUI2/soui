#include <helper\SCriticalSection.h>

namespace SOUI
{
	SCriticalSection::SCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	}
	SCriticalSection::~SCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	}

	void SCriticalSection::Enter()
	{
		EnterCriticalSection(&m_cs);
	}

	void SCriticalSection::Leave()
	{
		LeaveCriticalSection(&m_cs);
	}
}