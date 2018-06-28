#pragma once

#define MAX_COUNT 10 

/*
by 多点
@20150510 qq:4361031
*/
using namespace SOUI;

class SDesktopDock : public SWindow
{
	SOUI_CLASS_NAME(SDesktopDock, L"deskdock")
public:
	SDesktopDock();
protected:
	virtual void OnPaint(IRenderTarget *pRT);
	virtual void OnMouseMove(UINT nFlags,CPoint pt);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);

	void GetImageRate();
	void GetDockLen();
	void GetRTRect();
	void initDockRect();

protected:
	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"skin1", m_pSkin[0], TRUE)
		ATTR_SKIN(L"skin2", m_pSkin[1], TRUE)
		ATTR_SKIN(L"skin3", m_pSkin[2], TRUE)
		ATTR_SKIN(L"skin4", m_pSkin[3], TRUE)
		ATTR_SKIN(L"skin5", m_pSkin[4], TRUE)
		ATTR_SKIN(L"skin6", m_pSkin[5], TRUE)
		ATTR_SKIN(L"skin7", m_pSkin[6], TRUE)
		ATTR_SKIN(L"skin8", m_pSkin[7], TRUE)
		ATTR_SKIN(L"skin9", m_pSkin[8], TRUE)
		ATTR_SKIN(L"skin10", m_pSkin[9], TRUE)

		SOUI_ATTRS_END()

		SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		SOUI_MSG_MAP_END()
		//////////////////////////////////////////////////////////////////////////

private:
	ISkinObj *m_pSkin[MAX_COUNT];

	float m_iRotate;
	int   m_CPX;
	int   m_CPY;
	double m_DockLen;
	int   m_Width;
	int   m_Height;
	double m_MaxDisc;
	double m_MinDisc;
	double m_MaxRate;
	double m_MinRate;
	int m_gap;

	double m_dRate[MAX_COUNT];
	CRect  m_RTRect[MAX_COUNT];

	BOOL m_Init;

};
