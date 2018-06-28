#pragma once

namespace SOUI
{

    struct S_UPDATELAYEREDWINDOWINFO {
        DWORD cbSize;
        HDC hdcDst;
        POINT *pptDst;
        SIZE *psize;
        HDC hdcSrc;
        POINT *pptSrc;
        COLORREF crKey;
        BLENDFUNCTION *pblend;
        DWORD dwFlags;
        RECT *prcDirty;
    };

    class SWndSurface{
    public:
        static BOOL Init();
        static BOOL WINAPI SUpdateLayeredWindowIndirect(HWND hWnd, const S_UPDATELAYEREDWINDOWINFO *pULWInfo);        
    };
}
