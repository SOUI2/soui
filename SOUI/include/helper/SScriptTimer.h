#pragma once

#include "core/SSingletonMap.h"


namespace SOUI
{

    struct SCRIPTTIMERINFO
    {
        HWND hwnd;
        SStringA strScriptFunc;
        BOOL bRepeat;
    };

    class SScriptTimer : public SSingletonMap<SScriptTimer,SCRIPTTIMERINFO,UINT_PTR>
    {
		SINGLETON2_TYPE(SINGLETON_SCRIPTTIMER)
    public:
        ~SScriptTimer();

        UINT SetTimer(HWND hwnd,const SStringA & strScriptFunc,UINT nElapse,BOOL bRepeat);

		void ClearTimer(UINT_PTR uID);

        static VOID CALLBACK _TimerProc(HWND hwnd,
            UINT uMsg,
            UINT_PTR idEvent,
            DWORD dwTime
            );
    };

}//namespace SOUI