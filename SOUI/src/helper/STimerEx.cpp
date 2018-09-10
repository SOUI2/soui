#include "souistd.h"
#include "helper/STimerEx.h"
#include "core/SWnd.h"
#include "core/SWindowMgr.h"

namespace SOUI
{


//////////////////////////////////////////////////////////////////////////
//    STimer2
//////////////////////////////////////////////////////////////////////////

BOOL STimer2::_SetTimer( SWND swnd,UINT_PTR uTimerID,UINT nElapse )
{
    _KillTimer(swnd,uTimerID);
    UINT_PTR idEvent=::SetTimer(NULL,uTimerID,nElapse,_TimerProc);
    if(idEvent==0) return FALSE;
    TIMERINFO ti= {swnd,uTimerID};
    (*m_mapNamedObj)[idEvent]=ti;
    return TRUE;
}

void STimer2::_KillTimer( SWND swnd,UINT_PTR uTimerID )
{
    SPOSITION pos=m_mapNamedObj->GetStartPosition();
    while(pos)
    {
        SMap<UINT_PTR,TIMERINFO>::CPair *p=m_mapNamedObj->GetNext(pos);
        if(p->m_value.Swnd==swnd && p->m_value.uTimerID==uTimerID)
        {
            ::KillTimer(NULL,p->m_key);
            m_mapNamedObj->RemoveAtPos((SPOSITION)p);
            break;
        }
    }
}

void STimer2::_KillTimer( SWND Swnd )
{
    SPOSITION pos=m_mapNamedObj->GetStartPosition();
    while(pos)
    {
        SMap<UINT_PTR,TIMERINFO>::CPair *p=m_mapNamedObj->GetNext(pos);
        if(p->m_value.Swnd==Swnd)
        {
            ::KillTimer(NULL,p->m_key);
            m_mapNamedObj->RemoveAtPos((SPOSITION)p);
        }
    }
}


VOID CALLBACK STimer2::_TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
    TIMERINFO ti;
    if(getSingleton().GetKeyObject(idEvent,ti))
    {
        SWindow *pSwnd=SWindowMgr::GetWindow(ti.Swnd);
        if(pSwnd) pSwnd->SSendMessage(WM_TIMER2,ti.uTimerID);
    }
}

}//namespace SOUI