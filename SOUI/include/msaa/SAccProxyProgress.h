#pragma once

#include "SAccProxyWindow.h"

namespace SOUI
{
	class SAccProxyProgress : public SAccProxyWindow
	{
	public:
		SAccProxyProgress(SWindow *pWnd);
		~SAccProxyProgress(void);

	public:
		virtual STDMETHODIMP get_accName(BSTR *pszValue);
		virtual STDMETHODIMP get_accValue(BSTR *pszValue);
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
	};
}
