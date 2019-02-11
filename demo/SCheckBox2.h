#pragma once

namespace SOUI
{

	class SCheckBox2 : public SWindow
	{
		SOUI_CLASS_NAME(SCheckBox2,L"check2")
	public:
		SCheckBox2(void);
		~SCheckBox2(void);

	protected:
		HRESULT OnAttrCheck(const SStringW& strValue, BOOL bLoading);

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"checked",OnAttrCheck)
		SOUI_ATTRS_END()

	protected:
		void OnLButtonUp(UINT nFlags, CPoint point);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
		SOUI_MSG_MAP_END()

	};


}
