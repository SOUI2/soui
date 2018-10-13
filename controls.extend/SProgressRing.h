#pragma once

namespace SOUI
{
	class SProgressRing : public SProgress
	{
		SOUI_CLASS_NAME(SProgressRing,L"progressRing")
	public:
		SProgressRing(void);
		~SProgressRing(void);

		SOUI_ATTRS_BEGIN()
			ATTR_COLOR(L"colorBack", m_crBackground, TRUE)
			ATTR_COLOR(L"colorFore", m_crForeground, TRUE)
			ATTR_FLOAT(L"arcRate",m_fArcRate,TRUE)
		SOUI_ATTRS_END()

	protected:
		virtual void OnColorize(COLORREF cr);
	protected:
		void OnPaint(IRenderTarget *pRT);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()


		COLORREF m_crBackground;
		COLORREF m_crForeground;
		float m_fArcRate;
	};

}

