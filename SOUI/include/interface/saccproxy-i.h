#pragma once

#include <unknown/obj-ref-i.h>

namespace SOUI
{
	struct IAccProxy : IObjRef
	{
		virtual STDMETHODIMP get_accName(BSTR *pszName) = 0;
		virtual STDMETHODIMP get_accValue(BSTR *pszValue) = 0;
		virtual STDMETHODIMP get_accDescription(BSTR *pszDescription) = 0;
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole) = 0;
		virtual STDMETHODIMP get_accState(VARIANT *pvarState) = 0;
		virtual STDMETHODIMP get_accHelp(BSTR *pszHelp) = 0;
		virtual STDMETHODIMP get_accHelpTopic(BSTR *pszHelpFile, long *pidTopic) = 0;
		virtual STDMETHODIMP get_accKeyboardShortcut(BSTR *pszKeyboardShortcut) = 0;
		virtual STDMETHODIMP get_accDefaultAction(BSTR *pszDefaultAction) = 0;
		virtual STDMETHODIMP accSelect(long flagsSelect) = 0;
		virtual STDMETHODIMP accDoDefaultAction() = 0;
		virtual STDMETHODIMP put_accName(BSTR szName) = 0;
		virtual STDMETHODIMP put_accValue(BSTR szValue) = 0;
	};

}