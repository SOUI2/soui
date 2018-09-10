#pragma once

#include "core/SSingletonMap.h"

namespace SOUI
{

typedef struct tagTIMERINFO
{
    SWND Swnd;
    UINT_PTR uTimerID;
} TIMERINFO;


class SOUI_EXP STimer2:public SSingletonMap<STimer2,TIMERINFO,UINT_PTR>
{
	SINGLETON2_TYPE(SINGLETON_TIMER)
public:
    static BOOL SetTimer(SWND swnd,UINT_PTR uTimerID,UINT nElapse)
    {
        return getSingleton()._SetTimer(swnd,uTimerID,nElapse);
    }

    static void KillTimer(SWND swnd,UINT_PTR uTimerID)
    {
        getSingleton()._KillTimer(swnd,uTimerID);
    }

    static void KillTimer(SWND swnd)
    {
        getSingleton()._KillTimer(swnd);
    }
protected:
    BOOL _SetTimer(SWND swnd,UINT_PTR uTimerID,UINT nElapse);

    void _KillTimer(SWND swnd,UINT_PTR uTimerID);

    void _KillTimer(SWND swnd);

    static VOID CALLBACK _TimerProc(HWND hwnd,
                                    UINT uMsg,
                                    UINT_PTR idEvent,
                                    DWORD dwTime
                                   );
};

}//namespace SOUI