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
		if (!m_pWnd->GetParent())
		{
			return E_INVALIDARG;
		}
		return m_pWnd->GetParent()->GetAccessible()->QueryInterface(IID_IDispatch, (void**)ppdispParent);
	}

	HRESULT SAccessible::get_accChild(VARIANT varChild, IDispatch **ppdispChild)
	{
		if (varChild.vt != VT_I4 || !ppdispChild) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		SLOG_INFO("getChild:" << varChild.lVal << " pChild:" << pChild);
		if (!pChild) return E_INVALIDARG;
		return pChild->GetAccessible()->QueryInterface(IID_IDispatch, (void**)ppdispChild);
	}

	HRESULT SAccessible::get_accChildCount(long *pcountChildren)
	{
		SLOG_INFO("getChildCount:" << m_pWnd->GetChildrenCount());
		*pcountChildren = m_pWnd->GetChildrenCount();
		return S_OK;
	}

	HRESULT SAccessible::get_accValue(VARIANT varChild, BSTR *pszValue)
	{
		if (varChild.vt != VT_I4 || !pszValue)
			return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		*pszValue = ::SysAllocString(S_CT2W(pChild->GetWindowText()));
		return S_OK;
	}

	HRESULT SAccessible::put_accValue(VARIANT varChild, BSTR szValue)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		pChild->SetWindowText(S_CW2T(szValue));
		return S_OK;
	}

	HRESULT SAccessible::get_accName(VARIANT varChild, BSTR *pszName)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		SStringW strName = pChild->GetName();
		if (!strName.IsEmpty())
			*pszName = ::SysAllocString(strName);
		else
			*pszName = ::SysAllocString(SStringW().Format(L"%s_%d", pChild->GetObjectClass(), pChild->GetID()));
		return S_OK;
	}

	HRESULT SAccessible::put_accName(VARIANT varChild, BSTR szName)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		pChild->SetName(szName);
		return S_OK;
	}

	HRESULT SAccessible::accDoDefaultAction(VARIANT varChild)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		pChild->FireCommand();
		return S_OK;
	}

	HRESULT SAccessible::get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		*pszDefaultAction = ::SysAllocString(pChild->GetClassName());
		return S_OK;
	}

	HRESULT SAccessible::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
	{
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
			SLOGFMTF("this:%08x,vt:%d,lVal:0",this, VT_I4);
		}
		else
		{
			pvarChild->vt = VT_DISPATCH;
			pChild->GetAccessible()->QueryInterface(IID_IDispatch, (void**)&pvarChild->pdispVal);
			SLOGFMTF("this:%08x,vt:%d,disp:0x%08x", this, VT_DISPATCH,pvarChild->pdispVal);
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

		default:
			return(FALSE);
		}

		return(TRUE);
	}

	HRESULT SAccessible::accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt)
	{
		SLOG_INFO("accNavigate, navDir:" << navDir << " start:" << varStart.lVal);

		HRESULT hr = E_INVALIDARG;
		pvarEndUpAt->vt = VT_EMPTY;
		if (!accValidateNavStart(&varStart))
			return hr;
		switch (navDir)
		{
		case NAVDIR_FIRSTCHILD:
			if (varStart.vt != VT_I4 || varStart.lVal != CHILDID_SELF)
				break;
			if (m_pWnd->GetChildrenCount() == 0) break;
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = 1;
			break;
		case NAVDIR_LASTCHILD:
			if (varStart.vt != VT_I4 || varStart.lVal != CHILDID_SELF)
				break;
			if (m_pWnd->GetChildrenCount() == 0) break;
			pvarEndUpAt->vt = VT_I4;
			pvarEndUpAt->lVal = m_pWnd->GetChildrenCount();
			break;
		case NAVDIR_DOWN:
		case NAVDIR_RIGHT:
		case NAVDIR_NEXT:
			{
				if (varStart.vt != VT_I4 || m_pWnd->GetChildrenCount() == 0) break;
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = varStart.lVal + 1;
				if (pvarEndUpAt->lVal > m_pWnd->GetChildrenCount())
					pvarEndUpAt->lVal = 1;
				hr = S_OK;
			}
			break;
		case NAVDIR_UP:
		case NAVDIR_LEFT:
		case NAVDIR_PREVIOUS:
			{
				if (varStart.vt != VT_I4 || m_pWnd->GetChildrenCount() == 0) break;
				pvarEndUpAt->vt = VT_I4;
				pvarEndUpAt->lVal = varStart.lVal - 1;
				if (pvarEndUpAt->lVal < 1)
					pvarEndUpAt->lVal = m_pWnd->GetChildrenCount();
				hr = S_OK;
			}
			break;

		}
		SLOG_INFO("accNavigate, navDir:" << navDir << " start:" << varStart.lVal << " EndUpAt:" << pvarEndUpAt->lVal);
		return hr;
	}

	HRESULT SAccessible::accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
	{
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
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		if (((flagsSelect & SELFLAG_TAKEFOCUS) > 0) && (::GetFocus() == m_pWnd->GetContainer()->GetHostHwnd()))
		{
			pChild->SetFocus();
			return S_OK;
		}
		return S_FALSE;
	}

	HRESULT SAccessible::get_accSelection(VARIANT *pvarChildren)
	{
		return E_NOTIMPL;
	}


	HRESULT SAccessible::get_accFocus(VARIANT *pvarChild)
	{
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
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;

		DWORD dwState = pChild->GetState();

		pvarState->vt = VT_I4;
		pvarState->lVal = 0;
		if (!pChild->IsVisible(TRUE))
			pvarState->lVal = STATE_SYSTEM_INVISIBLE;
		else if (pChild->IsDisabled(TRUE))
			pvarState->lVal = STATE_SYSTEM_UNAVAILABLE;
		else {
			if (dwState & WndState_PushDown)
				pvarState->lVal = STATE_SYSTEM_PRESSED;
			if (dwState & WndState_Check)
				pvarState->lVal |= STATE_SYSTEM_CHECKED;

			if (m_pWnd->GetContainer()->GetFocus() == pChild->GetSwnd())
				pvarState->lVal |= STATE_SYSTEM_FOCUSED;
			if (pChild->IsFocusable())
				pvarState->lVal |= STATE_SYSTEM_FOCUSABLE;
		}

		return S_OK;
	}

	STDMETHODIMP SAccessible::get_accHelp(VARIANT varChild, BSTR * pszHelp)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP SAccessible::get_accHelpTopic(BSTR * pszHelpFile, VARIANT varChild, long * pidTopic)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP SAccessible::get_accKeyboardShortcut(VARIANT varChild, BSTR * pszKeyboardShortcut)
	{
		return E_NOTIMPL;
	}


	HRESULT SAccessible::get_accRole(VARIANT varChild, VARIANT *pvarRole)
	{
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		pvarRole->vt = VT_I4;
		pvarRole->lVal = pChild->accRole();
		return S_OK;
	}

	HRESULT SAccessible::get_accDescription(VARIANT varChild, BSTR *pszDescription)
	{
#ifdef _DEBUG
		if (varChild.vt != VT_I4) return E_INVALIDARG;
		SWindow *pChild = m_pWnd->GetChild(varChild.lVal);
		if (!pChild) return E_INVALIDARG;
		*pszDescription = ::SysAllocString(m_pWnd->m_strXml);
		return S_OK;
#else
		return E_NOTIMPL;
#endif
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


#endif//SOUI_ENABLE_ACC
}

