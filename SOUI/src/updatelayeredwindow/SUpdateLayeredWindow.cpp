#include "souistd.h"
#include "SUpdateLayeredWindow.h"

namespace SOUI{

    typedef BOOL (WINAPI *FunUpdateLayeredWindow)(HWND hwnd, HDC hdcDst, const POINT *pptDst,
        const SIZE *psize, HDC hdcSrc, const POINT *pptSrc, COLORREF crKey,
        const BLENDFUNCTION *pblend, DWORD dwflags);
    typedef BOOL (WINAPI *FunUpdateLayeredWindowIndirect)(HWND hwnd, const S_UPDATELAYEREDWINDOWINFO *pULWInfo);

    static FunUpdateLayeredWindow  s_funUpdateLayeredWindow=NULL;
    static FunUpdateLayeredWindowIndirect s_funUpdateLayeredWindowIndirect=NULL;

    
    BOOL WINAPI _SUpdateLayeredWindowIndirect(HWND hWnd, const S_UPDATELAYEREDWINDOWINFO *info)
    {
        SASSERT(s_funUpdateLayeredWindow);
        return (*s_funUpdateLayeredWindow)(hWnd, info->hdcDst, info->pptDst, info->psize, info->hdcSrc,
            info->pptSrc, info->crKey, info->pblend, info->dwFlags);
    }

    BOOL SWndSurface::Init()
    {
        HMODULE hUser32 = GetModuleHandle(_T("user32"));
        if(!hUser32)
        {
            SASSERT(FALSE);
            return FALSE;
        }
        s_funUpdateLayeredWindow = (FunUpdateLayeredWindow)GetProcAddress(hUser32,"UpdateLayeredWindow");
        if(!s_funUpdateLayeredWindow)
        {
            SASSERT(FALSE);
            return FALSE;
        }
#ifdef _WIN64   //X64中使用UpdateLayeredWindowIndirect在Win7测试显示不出内容，只能使用UpdateLayeredWindow,fuck MS.
        s_funUpdateLayeredWindowIndirect = _SUpdateLayeredWindowIndirect;
#else
        s_funUpdateLayeredWindowIndirect = (FunUpdateLayeredWindowIndirect)GetProcAddress(hUser32,"UpdateLayeredWindowIndirect");
        if(!s_funUpdateLayeredWindowIndirect) s_funUpdateLayeredWindowIndirect = _SUpdateLayeredWindowIndirect;
#endif
        return TRUE;
    }

    BOOL WINAPI SWndSurface::SUpdateLayeredWindowIndirect(HWND hWnd, const S_UPDATELAYEREDWINDOWINFO *pULWInfo)
    {
        BOOL bRet = s_funUpdateLayeredWindowIndirect(hWnd,pULWInfo);
        return bRet;
    }

}
