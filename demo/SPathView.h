#pragma once
namespace SOUI{

	class EventPath : public TplEventArgs<EventPath>
	{
		SOUI_CLASS_NAME(EventPath,L"on_event_path")
	public:
		enum {EventID=EVT_EXTERNAL_BEGIN+23451};
		EventPath(SObject *pSender):TplEventArgs<EventPath>(pSender){}

		float fLength;
	};

	class SPathView : public SWindow
	{
		SOUI_CLASS_NAME(SPathView,L"pathView")
	public:
		SPathView(void);
		~SPathView(void);

		void AddPoint(const POINT *pts,int nPts);

		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"lineWidth",m_nLineWidth,TRUE)
			ATTR_INT(L"lineStyle",m_nLineStyle,TRUE)
			ATTR_COLOR(L"lineColor",m_crLine,TRUE)
			ATTR_INT(L"frameSize",m_nFrameSize,TRUE)
			ATTR_FLOAT(L"cornerRadius",m_fCornerRadius,TRUE)
		SOUI_ATTRS_END()
	protected:
		void OnLButtonDown(UINT nFlags,CPoint pt);
		void OnRButtonDown(UINT nFlags, CPoint point);
		void OnPaint(IRenderTarget *pRT);
		void OnPathChanged();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_RBUTTONDOWN(OnRButtonDown)
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	protected:
		int m_nLineWidth;
		int m_nLineStyle;
		COLORREF m_crLine;
		int m_nFrameSize;
		float m_fCornerRadius;

		SArray<POINT> m_lstPoints;
		CAutoRefPtr<IPath> m_path;
		CAutoRefPtr<IPathMeasure> m_pathMeasure;
	};


}
