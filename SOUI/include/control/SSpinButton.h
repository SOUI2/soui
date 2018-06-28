#pragma once
#include "core/swnd.h"

namespace SOUI
{
class SSpinButton : public SWindow, public ITimelineHandler
{
	SOUI_CLASS_NAME(SSpinButton, L"spin")
public:
	SSpinButton();
	~SSpinButton();
	void SetBuddy(SWindow* pBuddy);
protected:
	int HitTest(CPoint pt);
	void Start();
	void Stop();
	bool IsPlaying();
	void ChangeValue(int nHover);
protected:
	virtual void OnNextFrame();
protected:
    LRESULT OnCreate(LPVOID);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnDestroy();
	void OnPaint(IRenderTarget *pRT);
	void OnLButtonDown(UINT nFlags, CPoint pt);
	void OnLButtonUp(UINT nFlags, CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
    //void ResizeHostWindow(int nOffset);
	HRESULT OnAttrBuddy(const SStringW& strValue, BOOL bLoading);

    SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"spinSkin", m_pSpinSkin, TRUE)
        ATTR_INT(L"min", m_nMinValue, FALSE)
        ATTR_INT(L"max", m_nMaxValue, FALSE)
		ATTR_UINT(L"step", m_uStep, FALSE)
		ATTR_BOOL(L"circle", m_bCircle, FALSE)
		ATTR_CUSTOM(L"buddy", OnAttrBuddy)
	SOUI_ATTRS_END()

	SOUI_MSG_MAP_BEGIN()
        MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDown) //将双击消息处理为单击
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
	SOUI_MSG_MAP_END()

    int										m_nMinValue;         /**< 最小大小 */
    int										m_nMaxValue;         /**< 最大大小 */
	UINT									m_uStep;
	bool									m_bCircle;
	ISkinObj*							m_pSpinSkin;
	int										m_nHover;			// -1 none  1 left top		2 right bottom
	int										m_nPush;

	SWindow*							m_pBuddy;

	bool									m_bFirst;
	int										m_iNextTime;
};

};