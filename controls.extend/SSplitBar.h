#pragma once
#include <core/swnd.h>

namespace SOUI
{
class SSplitBar : public SWindow
{
    SOUI_CLASS_NAME(SSplitBar, L"splitbar")
public:
    SSplitBar();
    ~SSplitBar();
    
protected:
    void    OnLButtonDown(UINT nFlags,CPoint pt);
    void    OnLButtonUp(UINT nFlags,CPoint pt);
    void    OnMouseMove(UINT nFlags,CPoint pt);
    BOOL    OnSetCursor(const CPoint &pt);
    LRESULT OnCreate( LPVOID );

    void ResizeHostWindow(int nOffset);
    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"resizeHostWnd", m_bResizeHostWnd, FALSE)
        ATTR_INT(L"vertical", m_bVertical, FALSE)
        ATTR_INT(L"minSize", m_nSizeMin, FALSE)
        ATTR_INT(L"maxSize", m_nSizeMax,FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_CREATE(OnCreate)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
    SOUI_MSG_MAP_END()

    CPoint      m_ptDragPrev;       /**< 拖动调整的前一个位置 */
    BOOL        m_bDragging;        /**< 正在拖动 */
    BOOL        m_bResizeHostWnd;   /**< 是否缩放宿主窗口*/
    int         m_nSizeMin;         /**< 最小大小 */
    int         m_nSizeMax;         /**< 最大大小 */
    int         m_nOrginPos;        /**< 最原始的left/top*/
    int         m_nTrackingPos;
    BOOL        m_bVertical;        /**< 是否垂直分割，默认是水平分割*/
};

};