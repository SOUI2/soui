#include "souistd.h"
#include "core/SwndAccessible.h"
#pragma comment(lib,"oleacc.lib")

namespace SOUI
{
#ifdef SOUI_ENABLE_ACC
	SAccessible::SAccessible(SWindow * pWnd):m_pWnd(pWnd)
	{
	}


	SAccessible::~SAccessible()
	{
	}

	HRESULT SAccessible::get_accParent(IDispatch **ppdispParent)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (!m_pWnd->GetParent())
		{
			HWND hParent = ::GetParent(m_pWnd->GetContainer()->GetHostHwnd());
			if(hParent)
				return AccessibleObjectFromWindow(hParent, OBJID_WINDOW,IID_IAccessible, (void**)ppdispParent);
			else
				return E_INVALIDARG;
		}
		SComPtr<IAccessible> pAcc = m_pWnd->GetParent()->GetAccessible();
		if(!pAcc) return E_NOINTERFACE;
		return pAcc->QueryInterface(IID_IDispatch, (void**)ppdispParent);
	}

	HRESULT SAccessible::get_accChild(VARIANT varChild, IDispatch **ppdispChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4 || !ppdispChild) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		SComPtr<IAccessible> pAcc = pChild->GetAccessible();
		if(!pAcc) return E_NOINTERFACE;
		return pAcc->QueryInterface(IID_IDispatch, (void**)ppdispChild);
	}

	HRESULT SAccessible::get_accChildCount(long *pcountChildren)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		*pcountChildren = m_pWnd->GetChildrenCount();
		return S_OK;
	}

	HRESULT SAccessible::get_accValue(VARIANT varChild, BSTR *pszValue)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4 || !pszValue)
			return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->get_accValue(pszValue);
	}

	HRESULT SAccessible::put_accValue(VARIANT varChild, BSTR szValue)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->put_accValue(szValue);
	}

	HRESULT SAccessible::get_accName(VARIANT varChild, BSTR *pszName)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		SASSERT(SWindowMgr::GetWindow(m_pWnd->GetSwnd())==m_pWnd);
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->get_accName(pszName);
	}

	HRESULT SAccessible::put_accName(VARIANT varChild, BSTR szName)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->put_accName(szName);
	}

	HRESULT SAccessible::accDoDefaultAction(VARIANT varChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->accDoDefaultAction();
	}

	HRESULT SAccessible::get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->get_accDefaultAction(pszDefaultAction);
	}

	HRESULT SAccessible::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (!pvarChild) return E_INVALIDARG;
		CPoint pt(xLeft, yTop);
		ScreenToClient(m_pWnd->GetContainer()->GetHostHwnd(), &pt);

		SWindow *pChild = m_pWnd->GetWindow(GSW_LASTCHILD);
		while (pChild)
		{
			if (pChild->IsVisible(TRUE) && pChild->IsContainPoint(pt,FALSE))
				break;
			pChild = pChild->GetWindow(GSW_PREVSIBLING);
		}
		if (!pChild)
		{
			pvarChild->vt = VT_I4;
			pvarChild->lVal = CHILDID_SELF;
		}
		else
		{
			pvarChild->vt = VT_DISPATCH;
			pChild->GetAccessible()->QueryInterface(IID_IDispatch, (void**)&pvarChild->pdispVal);
		}
		return S_OK;
	}

	// --------------------------------------------------------------------------
	//
	//  ValidateChild()
	//
	//  The window children are the OBJID_s of the elements that compose the
	//  frame.  These are NEGATIVE values.  Hence we override the validation.
	//
	// --------------------------------------------------------------------------
	BOOL SAccessible::accValidateNavStart(VARIANT* pvar) const
	{
		//
		// This validates a VARIANT parameter and translates missing/empty
		// params.
		//

	TryAgain:
		// Missing parameter, a la VBA
		switch (pvar->vt)
		{
		case VT_VARIANT | VT_BYREF:
			VariantCopy(pvar, pvar->pvarVal);
			goto TryAgain;

		case VT_ERROR:
			if (pvar->scode != DISP_E_PARAMNOTFOUND)
				return(FALSE);
			// FALL THRU

		case VT_EMPTY:
			pvar->vt = VT_I4;
			pvar->lVal = 0;
			break;
		case VT_I4:
			if ((pvar->lVal < 0) || (pvar->lVal >(LONG)m_pWnd->GetChildrenCount()))
				return(FALSE);
			break;

		case VT_DISPATCH:
			return TRUE;
		default:
			return(FALSE);
		}

		return(TRUE);
	}

	HRESULT SAccessible::accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		HRESULT hr = E_INVALIDARG;
		pvarEndUpAt->vt = VT_EMPTY;
		if (!accValidateNavStart(&varStart))
			return hr;
		switch (navDir)
		{
		case NAVDIR_FIRSTCHILD:
			if(m_pWnd->GetChildrenCount() == 0) break;
			if((varStart.vt == VT_DISPATCH && varStart.pdispVal == NULL) 
				|| (varStart.vt == VT_I4 && varStart.lVal == CHILDID_SELF))
			{
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = 1;
			}
			hr = S_OK;
			break;
		case NAVDIR_LASTCHILD:
			if(m_pWnd->GetChildrenCount() == 0) break;
			if((varStart.vt == VT_DISPATCH && varStart.pdispVal == NULL) 
				|| (varStart.vt == VT_I4 && varStart.lVal == CHILDID_SELF))
			{
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = m_pWnd->GetChildrenCount();
			}
			hr = S_OK;
			break;
		case NAVDIR_DOWN:
		case NAVDIR_RIGHT:
		case NAVDIR_NEXT:
			if(varStart.vt == VT_DISPATCH)
			{
				SAccessible *pAcc = varStart.pdispVal?(SAccessible*)varStart.pdispVal:this;
				SWindow * pNext =  pAcc->m_pWnd->GetWindow(GSW_NEXTSIBLING);
				if(pNext)
				{
					SComPtr<IAccessible> pAcc = pNext->GetAccessible();
					if(pAcc)
					{
						pvarEndUpAt->vt = VT_DISPATCH;
						pAcc->QueryInterface(IID_IDispatch,(void **)pvarEndUpAt->pdispVal);
					}
				}
				hr = S_OK;
			}else if(varStart.vt == VT_I4)
			{
				if (m_pWnd->GetChildrenCount() == 0) break;
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = varStart.lVal + 1;
				if (pvarEndUpAt->lVal > (int)m_pWnd->GetChildrenCount())
					pvarEndUpAt->lVal = 1;
				hr = S_OK;
			}
			break;
		case NAVDIR_UP:
		case NAVDIR_LEFT:
		case NAVDIR_PREVIOUS:
			if(varStart.vt == VT_DISPATCH)
			{
				SAccessible *pAcc = varStart.pdispVal?(SAccessible*)varStart.pdispVal:this;
				SWindow * pNext =  pAcc->m_pWnd->GetWindow(GSW_PREVSIBLING);
				if(pNext)
				{
					SComPtr<IAccessible> pAcc = pNext->GetAccessible();
					if(pAcc)
					{
						pvarEndUpAt->vt = VT_DISPATCH;
						pAcc->QueryInterface(IID_IDispatch,(void **)pvarEndUpAt->pdispVal);
					}
				}
				hr = S_OK;
			}else if(varStart.vt == VT_I4){
				if (m_pWnd->GetChildrenCount() == 0) break;
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = varStart.lVal - 1;
				if (pvarEndUpAt->lVal < 1)
					pvarEndUpAt->lVal = m_pWnd->GetChildrenCount();
				hr = S_OK;
			}
			break;

		}
		return hr;
	}

	HRESULT SAccessible::accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (!(varChild.vt == VT_I4 && pxLeft &&pyTop && pcxWidth && pcyHeight)) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		CRect rc = pChild->GetWindowRect();
		CPoint pt = rc.TopLeft();
		ClientToScreen(m_pWnd->GetContainer()->GetHostHwnd(), &pt);
		*pxLeft = pt.x;
		*pyTop = pt.y;
		*pcxWidth = rc.Width();
		*pcyHeight = rc.Height();
		return S_OK;
	}

	HRESULT SAccessible::accSelect(long flagsSelect, VARIANT varChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		return pChild->GetAccProxy()->accSelect(flagsSelect);
	}

	HRESULT SAccessible::get_accSelection(VARIANT *pvarChildren)
	{
		return E_NOTIMPL;
	}


	HRESULT SAccessible::get_accFocus(VARIANT *pvarChild)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		HWND hFocused = ::GetFocus();
		if (hFocused == m_pWnd->GetContainer()->GetHostHwnd())
		{
			SWND hFocus = m_pWnd->GetContainer()->GetFocus();
			SWindow *pFocus = SWindowMgr::GetWindow(hFocus);
			if (pFocus)
			{
				if (pFocus == m_pWnd)
				{
					pvarChild->vt = VT_I4;
					pvarChild->lVal = CHILDID_SELF;
				}
				else if (pFocus->GetParent() == m_pWnd)
				{
					int iChild = 0;
					SWindow *pChild = pFocus;
					while (pChild)
					{
						iChild++;
						pChild = pChild->GetWindow(GSW_PREVSIBLING);
					}
					pvarChild->vt = VT_I4;
					pvarChild->lVal = iChild;
				}
				else
				{
					pvarChild->vt = VT_DISPATCH;
					pFocus->GetAccessible()->QueryInterface(IID_IDispatch, (void**)&pvarChild->pdispVal);
				}
				return S_OK;
			}
			else
			{
				return S_FALSE;
			}
		}
		else
		{
			pvarChild->vt = VT_DISPATCH;
			return AccessibleObjectFromWindow(hFocused, OBJID_WINDOW,
				IID_IAccessible, (void**)&pvarChild->pdispVal);
		}
	}

	HRESULT SAccessible::get_accState(VARIANT varChild, VARIANT *pvarState)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		return pChild->GetAccProxy()->get_accState(pvarState);
	}

	STDMETHODIMP SAccessible::get_accHelp(VARIANT varChild, BSTR * pszHelp)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		return pChild->GetAccProxy()->get_accHelp(pszHelp);
	}

	STDMETHODIMP SAccessible::get_accHelpTopic(BSTR * pszHelpFile, VARIANT varChild, long * pidTopic)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		return pChild->GetAccProxy()->get_accHelpTopic(pszHelpFile,pidTopic);
	}

	STDMETHODIMP SAccessible::get_accKeyboardShortcut(VARIANT varChild, BSTR * pszKeyboardShortcut)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		return pChild->GetAccProxy()->get_accKeyboardShortcut(pszKeyboardShortcut);
	}


	HRESULT SAccessible::get_accRole(VARIANT varChild, VARIANT *pvarRole)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		IAccProxy *pAccProxy = pChild->GetAccProxy();
		return pAccProxy->get_accRole(pvarRole);
	}

	HRESULT SAccessible::get_accDescription(VARIANT varChild, BSTR *pszDescription)
	{
		if(!m_pWnd) return CO_E_OBJNOTCONNECTED;
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccProxy()->get_accDescription(pszDescription);
	}

	// Implement IDispatch
	STDMETHODIMP SAccessible::GetTypeInfoCount(unsigned int * pctinfo)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP SAccessible::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP SAccessible::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, unsigned int cNames, LCID lcid, DISPID * rgDispId)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP SAccessible::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, unsigned int * puArgErr)
	{
		return E_NOTIMPL;
	}


#endif
	//SOUI_ENABLE_ACC
}

