#pragma once
#include <helper/SMatrix.h>
namespace SOUI
{
	class S3DView : public SWindow,public ITimelineHandler
	{
		SOUI_CLASS_NAME(S3DView,L"S3DView")

	public:
		S3DView(void);
		~S3DView(void);

	protected:
		virtual void OnNextFrame();

		void OnShowWindow(BOOL bShow, UINT nStatus);
		BOOL OnEraseBkgnd(IRenderTarget *pRT){return TRUE;}
		void OnPaint(IRenderTarget *pRT);
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_ENUM_BEGIN(L"rotateDir",RotateDir,TRUE)
				ATTR_ENUM_VALUE(L"rotate_x",RotateX)
				ATTR_ENUM_VALUE(L"rotate_y",RotateY)
				ATTR_ENUM_VALUE(L"rotate_z",RotateZ)
			ATTR_ENUM_END(m_rotateDir)
			ATTR_INT(L"speed",m_nSpeed,FALSE)
			ATTR_SKIN(L"skin_fore",m_skinFore,TRUE)
			ATTR_SKIN(L"skin_back",m_skinBack,TRUE)
		SOUI_ATTRS_END()

	private:
		enum RotateDir{
			RotateX,
			RotateY,
			RotateZ,
		};

		RotateDir m_rotateDir;
		int  m_nRotate;
		int  m_iStep;
		int  m_nSpeed;
		CAutoRefPtr<ISkinObj> m_skinFore;
		CAutoRefPtr<ISkinObj> m_skinBack;
	};


class SMatrixWindow : public SWindow
{
SOUI_CLASS_NAME(SMatrixWindow,L"matrixWindow")
public:
    SMatrixWindow(void);
    ~SMatrixWindow(void);
    
protected:
    void OnPaint(IRenderTarget *pRT);
    BOOL OnEraseBkgnd(IRenderTarget *pRT){return TRUE;}

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
    SOUI_MSG_MAP_END()
protected:
    HRESULT OnAttrRotate(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrScale(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrSkew(const SStringW & strValue,BOOL bLoading);
    HRESULT OnAttrTranslate(const SStringW & strValue,BOOL bLoading);

    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"rotate",OnAttrRotate)
        ATTR_CUSTOM(L"scale",OnAttrScale)
        ATTR_CUSTOM(L"skew",OnAttrSkew)
        ATTR_CUSTOM(L"translate",OnAttrTranslate)
    SOUI_ATTRS_END()
    
    float m_fRotate;
    float m_fScaleX,m_fScaleY;
    float m_fSkewX, m_fSkewY;
    float m_fTransX, m_fTransY;
};

}