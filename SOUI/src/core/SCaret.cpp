#include "souistd.h"
#include "core\SCaret.h"

namespace SOUI{

    SCaret::SCaret(SWND swnd,HBITMAP hBmp,int nWidth,int nHeight)
        :m_owner(swnd)
    {
        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,nWidth,nHeight);
        m_bmpCaret = (IBitmap*) pRT->GetCurrentObject(OT_BITMAP);
        if(hBmp)
        {
            //以拉伸方式创建一个插入符位图
            HDC hdc=pRT->GetDC(0);
            HDC hdc2=CreateCompatibleDC(hdc);
            SelectObject(hdc2,hBmp);

            BITMAP bm;
            GetObject(hBmp,sizeof(bm),&bm);
            StretchBlt(hdc,0,0,nWidth,nHeight,hdc2,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
            DeleteDC(hdc2);
            pRT->ReleaseDC(hdc);
        }
        else
        {
            //创建一个黑色插入符的位图
			CRect rc(0, 0, nWidth, nHeight);
			pRT->FillSolidRect(&rc, RGBA(0, 0, 0, 0xFF));
        }
    }

    SCaret::~SCaret(void)
    {
    }

    RECT SCaret::Draw(IRenderTarget *pRT,int x, int y,BOOL bErase)
    {
        SWindow * pOwner = SWindowMgr::GetWindow(m_owner);
        SASSERT(pOwner);
        SASSERT(pOwner->IsFocusable());
        
        CAutoRefPtr<IRenderTarget> pRTCaret;
        GETRENDERFACTORY->CreateRenderTarget(&pRTCaret,0,0);
        pRTCaret->SelectObject(m_bmpCaret);

        CSize szCaret = m_bmpCaret->Size();
        CRect rcCaret(CPoint(x,y),szCaret);
        CRect rcWnd = pOwner->GetClientRect();
        SWindow *pWnd = pOwner->GetParent();
        while(pWnd)
        {
            CRect rcLimit = pWnd->GetClientRect();
            rcWnd = rcWnd & rcLimit;
            pWnd = pWnd->GetParent();
        }
        pOwner->GetContainer()->FrameToHost(rcWnd);
        CRect rcCaretShow = rcCaret & rcWnd;
        
        pRT->BitBlt(&rcCaretShow,pRTCaret,rcCaretShow.left - rcCaret.left,rcCaretShow.top - rcCaret.top,DSTINVERT);
        return rcCaretShow;
    }

}
