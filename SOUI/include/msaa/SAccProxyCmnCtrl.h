#pragma once

#include "SAccProxyWindow.h"

namespace SOUI
{
#ifdef SOUI_ENABLE_ACC
	class SOUI_EXP SAccProxyButton : public SAccProxyWindow
	{
	public:
		SAccProxyButton(SWindow *pWnd):SAccProxyWindow(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};

	class SOUI_EXP SAccProxyCheckButton : public SAccProxyWindow
	{
	public:
		SAccProxyCheckButton(SWindow *pWnd):SAccProxyWindow(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};

	class SOUI_EXP SAccProxyRadioButton : public SAccProxyWindow
	{
	public:
		SAccProxyRadioButton(SWindow *pWnd):SAccProxyWindow(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};


	class SOUI_EXP SAccProxyCombobox : public SAccProxyWindow
	{
	public:
		SAccProxyCombobox(SWindow *pWnd):SAccProxyWindow(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};

	class SOUI_EXP SAccProxyEdit : public SAccProxyWindow
	{
	public:
		SAccProxyEdit(SWindow *pWnd):SAccProxyWindow(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};

	class SOUI_EXP SAccProxyProgress : public SAccProxyWindow
	{
	public:
		SAccProxyProgress(SWindow *pWnd);
		~SAccProxyProgress(void);

	public:
		virtual STDMETHODIMP get_accName(BSTR *pszValue);
		virtual STDMETHODIMP get_accValue(BSTR *pszValue);
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};

	class SOUI_EXP SAccProxySlideBar : public SAccProxyProgress
	{
	public:
		SAccProxySlideBar(SWindow *pWnd):SAccProxyProgress(pWnd){}
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};
#endif
}
