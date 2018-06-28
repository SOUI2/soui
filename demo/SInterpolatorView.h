#pragma once

#include <interface/sinterpolator-i.h>
namespace SOUI
{
	class SInterpolatorView : public SSliderBar
		,protected ITimelineHandler
	{
		SOUI_CLASS_NAME(SInterpolatorView,L"interpolatorView")
	public:
		SInterpolatorView(void);
		~SInterpolatorView(void);

		void SetInterpolator(IInterpolator *pInterpolator);
	public:
		int OnCreate(void*);
		void OnDestroy();
		void OnPaint(IRenderTarget * pRT);
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	protected:
		virtual void OnNextFrame();
		
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"speed",m_nSpeed,FALSE)
		SOUI_ATTRS_END()
		
		CAutoRefPtr<IInterpolator> m_interpolator;
		int			m_nSpeed;
		int			m_iFrame;
		float		m_fTime;
	};
}
