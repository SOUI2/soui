#include "stdafx.h"
#include "SSplitBar.h"
namespace SOUI
{

SSplitBar::SSplitBar():m_bDragging(FALSE)
    ,m_bResizeHostWnd(FALSE)
    ,m_nSizeMin(0)
    ,m_nSizeMax(0)
    ,m_bVertical(FALSE)
{

}

SSplitBar::~SSplitBar()
{

}

LRESULT SSplitBar::OnCreate( LPVOID )
{
    if(0 != __super::OnCreate(NULL)) return 1;
    int pi = m_bVertical ? PI_LEFT : PI_TOP;
    m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->pos[pi].nPos;
    m_nTrackingPos = m_nOrginPos;

    return 0;
}

BOOL SSplitBar::OnSetCursor(const CPoint &pt)
{
    HCURSOR hCursor=GETRESPROVIDER->LoadCursor(m_bVertical?IDC_SIZEWE:IDC_SIZENS);
    SetCursor(hCursor);
    return TRUE;
}

void SSplitBar::OnLButtonDown(UINT nFlags,CPoint pt)
{
    SWindow::OnLButtonDown(nFlags, pt);

    m_ptDragPrev = pt;
    m_bDragging = TRUE;
}

void SSplitBar::OnLButtonUp(UINT nFlags,CPoint pt)
{
    SWindow::OnLButtonUp(nFlags, pt);

    m_bDragging = FALSE;
    int pi = m_bVertical ? PI_LEFT : PI_TOP;
    m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->pos[pi].nPos;
    m_nTrackingPos = m_nOrginPos;
}

void SSplitBar::OnMouseMove(UINT nFlags,CPoint pt)
{
    SWindow::OnMouseMove(nFlags, pt);

    static int nLastUpdateTicks = 0;

    int now = GetTickCount();
    if (now - nLastUpdateTicks < 30 || !m_bDragging)
    {
        return;
    }
    nLastUpdateTicks = now;

    int nWindowOffset = 0;
    int nOffset = 0;

    // 计算两次鼠标的位置偏差

    if (m_bVertical)
        nOffset = pt.x - m_ptDragPrev.x;
    else
        nOffset = pt.y - m_ptDragPrev.y;

    // 计算分隔栏新的位置

    //SwndLayout * pLayout = GetLayout();
    SouiLayoutParam *pSouiLayoutParam = GetLayoutParamT<SouiLayoutParam>();
    SouiLayoutParamStruct *pLayout = (SouiLayoutParamStruct*)pSouiLayoutParam->GetRawData();
    int pi = m_bVertical?PI_LEFT:PI_TOP;
    int nNewPos = m_nOrginPos + nOffset * pLayout->pos[pi].cMinus;

    /*
     *  - 有一种情况要特殊处理:既要修改hostwnd的尺寸,top/left又是以-XXX的方式定义。
     *    这里的逻辑比较复杂，我自己也搞得很晕@_@
     * 
     *  - 因为在需要修改宿主窗口的情况下，left/top如果以right/bottom为锚点，在修改了hostwnd的尺寸后,
     *    不需要再修改位置信息，否则在逻辑上位置信息会被叠加计算。
     *    举个例子：假如原始的y pos位置是-100，窗口的高度为300，此时y的位置是200(相对底部对齐)
     *    拖动分隔条往上移动了5个px，如果位置信息被改成了-105，窗口减少了5px，变成了295。最后计算出来
     *    的位置变成了190
     *
     *  - 在这种情况下，不受maxsize和minisize的控制。因为分隔栏和bottom/right的相对位置不变，所以从
     *    使用的角度讲，是没有超出 [maxsize,minisize] 的范围的
    */

    HWND hWnd = GetContainer()->GetHostHwnd();
    BOOL bZoomed = ::IsZoomed(hWnd);
    BOOL bResizeWnd = m_bResizeHostWnd && !bZoomed;
    if (!(bResizeWnd && pLayout->pos[pi].cMinus < 0))
    {
        if (nNewPos > m_nSizeMax)
            nNewPos = m_nSizeMax;

        if (nNewPos < m_nSizeMin)
            nNewPos = m_nSizeMin;

        if (nNewPos == pLayout->pos[pi].nPos)
            return;

        pLayout->pos[pi].nPos = nNewPos;
    }

    // 调整窗口

    nWindowOffset = nNewPos - m_nTrackingPos;
    nWindowOffset *= pLayout->pos[pi].cMinus;
    m_nTrackingPos = nNewPos;

    if (bResizeWnd)
        ResizeHostWindow(nWindowOffset);
    else
        RequestRelayout();      
}

void SSplitBar::ResizeHostWindow(int nOffset)
{
    if ( m_bResizeHostWnd && nOffset != 0)
    {
        HWND hWnd = GetContainer()->GetHostHwnd();

        CRect rcWnd;
        ::GetWindowRect(hWnd, rcWnd);

        if (m_bVertical)
        {
            ::MoveWindow(hWnd, 
                rcWnd.left, 
                rcWnd.top, 
                rcWnd.Width() + nOffset,
                rcWnd.Height(),
                TRUE);
        }
        else
        {
            ::MoveWindow(hWnd, 
                rcWnd.left, 
                rcWnd.top, 
                rcWnd.Width(),
                rcWnd.Height() + nOffset,
                TRUE);
        }
    }
}

}//namespace SOUI
