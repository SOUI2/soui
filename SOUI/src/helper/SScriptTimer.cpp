#include "souistd.h"
#include "helper/SScriptTimer.h"

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    //  SScriptTimer
    template<> SScriptTimer * SSingleton<SScriptTimer>::ms_Singleton = NULL;

    SScriptTimer::~SScriptTimer()
    {
        SPOSITION pos=m_mapNamedObj->GetStartPosition();
        while(pos)
        {
			SMap<UINT_PTR, SCRIPTTIMERINFO>::CPair *p = m_mapNamedObj->GetNext(pos);
            ::KillTimer(NULL,p->m_key);
        }
    }

    VOID CALLBACK SScriptTimer::_TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
    {
        SCRIPTTIMERINFO ti;
        if(SScriptTimer::getSingleton().GetKeyObject(idEvent,ti))
        {
            if(!ti.bRepeat)
            {
                SScriptTimer::getSingleton().ClearTimer(idEvent);
            }
            if(::IsWindow(ti.hwnd))
            {
                ::SendMessage(ti.hwnd,UM_SCRIPTTIMER,idEvent,(LPARAM)(LPCSTR)ti.strScriptFunc);
            }
        }else
        {
            SScriptTimer::getSingleton().ClearTimer(idEvent);
        }
    }

    void SScriptTimer::ClearTimer( UINT_PTR uID )
    {
        ::KillTimer(NULL,uID);
        RemoveKeyObject(uID);
    }

    UINT SScriptTimer::SetTimer( HWND hwnd,const SStringA & strScriptFunc,UINT nElapse,BOOL bRepeat )
    {
        SCRIPTTIMERINFO ti={hwnd,strScriptFunc,bRepeat};
        UINT_PTR uID = ::SetTimer(NULL,0,nElapse,_TimerProc);
        if(uID != 0)
        {
            AddKeyObject(uID,ti);
        }
        return (UINT)uID;
    }


}//namespace SOUI