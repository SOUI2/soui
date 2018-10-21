#include "souistd.h"
#include "msaa\SAccProxyCmnCtrl.h"
#include "control\SCmnCtrl.h"

namespace SOUI
{
#ifdef SOUI_ENABLE_ACC

	STDMETHODIMP SAccProxyButton::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_PUSHBUTTON;
		return S_OK;
	}


	STDMETHODIMP SAccProxyCombobox::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_COMBOBOX;
		return S_OK;
	}

	STDMETHODIMP SAccProxyCheckButton::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_CHECKBUTTON;
		return S_OK;
	}


	STDMETHODIMP SAccProxyRadioButton::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_RADIOBUTTON;
		return S_OK;
	}


	STDMETHODIMP SAccProxyEdit::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_TEXT;
		return S_OK;
	}


	///////////////////////////////////////////////////////////////////////////
	SAccProxyProgress::SAccProxyProgress(SWindow *pWnd):SAccProxyWindow(pWnd)
	{
	}

	SAccProxyProgress::~SAccProxyProgress(void)
	{
	}

	STDMETHODIMP SAccProxyProgress::get_accValue(BSTR *pszValue)
	{
		SProgress *pProg = sobj_cast<SProgress>(m_pWnd);
		int nMin,nMax;
		pProg->GetRange(&nMin,&nMax);
		if(nMax==nMin) return E_FAIL;
		int nProg = (pProg->GetValue()- nMin)*100/(nMax - nMin);
		SStringW strValue=SStringW().Format(L"%d%%",nProg);
		*pszValue = ::SysAllocString(strValue);
		return S_OK;
	}

	STDMETHODIMP SAccProxyProgress::get_accName(BSTR *pszValue)
	{
		return get_accValue(pszValue);
	}

	STDMETHODIMP SAccProxyProgress::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_PROGRESSBAR;
		return S_OK;
	}

	////////////////////////////////////////////////////////////////////////////////////
	STDMETHODIMP SAccProxySlideBar::get_accRole(VARIANT *pvarRole)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_SLIDER;
		return S_OK;
	}

#endif//SOUI_ENABLE_ACC




}
