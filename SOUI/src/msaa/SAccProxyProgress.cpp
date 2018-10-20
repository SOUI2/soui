#include "souistd.h"
#include "msaa\SAccProxyProgress.h"
#include "control\SCmnCtrl.h"

namespace SOUI
{
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


}
