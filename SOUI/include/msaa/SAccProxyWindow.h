#pragma once
#include "interface/saccproxy-i.h"

namespace SOUI
{
	class SOUI_EXP SAccProxyWindow : public TObjRefImpl<IAccProxy>
	{
	public:
		SAccProxyWindow(SWindow * pWnd) :m_pWnd(pWnd) {}
		virtual ~SAccProxyWindow(){}
	public:
		virtual STDMETHODIMP get_accName(BSTR *pszName);
		virtual STDMETHODIMP get_accValue(BSTR *pszValue);
		virtual STDMETHODIMP get_accDescription(BSTR *pszDescription);
		virtual STDMETHODIMP get_accRole(VARIANT *pvarRole);
		virtual STDMETHODIMP get_accState(VARIANT *pvarState);
		virtual STDMETHODIMP get_accHelp(BSTR *pszHelp);
		virtual STDMETHODIMP get_accHelpTopic(BSTR *pszHelpFile, long *pidTopic);
		virtual STDMETHODIMP get_accKeyboardShortcut(BSTR *pszKeyboardShortcut);
		virtual STDMETHODIMP get_accDefaultAction(BSTR *pszDefaultAction);
		virtual STDMETHODIMP accSelect(long flagsSelect);
		virtual STDMETHODIMP accDoDefaultAction();
		virtual STDMETHODIMP put_accName(BSTR szName);
		virtual STDMETHODIMP put_accValue(BSTR szValue);
	protected:
		SWindow * m_pWnd;
	};

}
