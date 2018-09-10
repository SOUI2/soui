#include "souistd.h"
#include "core/SWindowMgr.h"
#include "helper/STimerEx.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////
SWindowMgr::SWindowMgr()
    : m_hNextWnd(NULL)
{
    ::InitializeCriticalSection(&m_lockWndMap);
}

SWindowMgr::~SWindowMgr()
{
    ::DeleteCriticalSection(&m_lockWndMap);
}

// Get SWindow pointer from handle
SWindow* SWindowMgr::GetWindow(SWND swnd)
{
    if(!swnd) return NULL;
    SWindow *pRet=NULL;
    ::EnterCriticalSection(&getSingleton().m_lockWndMap);

    getSingleton().GetKeyObject(swnd,pRet);
    ::LeaveCriticalSection(&getSingleton().m_lockWndMap);
    return pRet;
}

// Specify a handle to a SWindow
SWND SWindowMgr::NewWindow(SWindow *pSwnd)
{
    SASSERT(pSwnd);
    ::EnterCriticalSection(&getSingleton().m_lockWndMap);

    SWND swndNext = ++ getSingleton().m_hNextWnd;
    getSingleton().AddKeyObject(swndNext,pSwnd);
    ::LeaveCriticalSection(&getSingleton().m_lockWndMap);

    return swndNext;
}

// Destroy DuiWindow
BOOL SWindowMgr::DestroyWindow(SWND swnd)
{
    ::EnterCriticalSection(&getSingleton().m_lockWndMap);

    BOOL bRet=getSingleton().RemoveKeyObject(swnd);
    STimer2::KillTimer(swnd);

    ::LeaveCriticalSection(&getSingleton().m_lockWndMap);

    return bRet;
}

}//namespace SOUI