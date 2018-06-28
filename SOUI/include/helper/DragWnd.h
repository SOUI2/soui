#pragma once

#include "core/SimpleWnd.h"
#include "helper/MemDC.h"

namespace SOUI
{

class CDragWnd : public CSimpleWnd
{
public:
    CDragWnd(void);
    ~CDragWnd(void);

    static BOOL BeginDrag(HBITMAP hBmp,POINT ptHot ,COLORREF crKey, BYTE byAlpha,DWORD dwFlags);
    static void DragMove(POINT pt);
    static void EndDrag();
protected:

    void OnPaint(HDC dc);

    BEGIN_MSG_MAP_EX(CDragWnd)
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()

    CPoint m_ptHot;
    HBITMAP m_bmp;

    static CDragWnd * s_pCurDragWnd;
};

}//end of namespace
