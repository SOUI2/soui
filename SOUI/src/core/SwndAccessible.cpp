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

	STDMETHODIMP SAccessible::get_accParent(IDispatch ** ppdispParent)
	{
		return m_pWnd->get_accParent(ppdispParent);
	}

	STDMETHODIMP SAccessible::get_accChildCount(long * pcountChildren)
	{
		return m_pWnd->get_accChildCount(pcountChildren);
	}

	STDMETHODIMP SAccessible::get_accChild(VARIANT varChild, IDispatch ** ppdispChild)
	{
		return m_pWnd->get_accChild(varChild, ppdispChild);
	}

	STDMETHODIMP SAccessible::get_accName(VARIANT varChild, BSTR * pszName)
	{
		return m_pWnd->get_accName(varChild, pszName);
	}

	STDMETHODIMP SAccessible::get_accValue(VARIANT varChild, BSTR * pszValue)
	{
		return m_pWnd->get_accValue(varChild, pszValue);
	}

	STDMETHODIMP SAccessible::get_accDescription(VARIANT varChild, BSTR * pszDescription)
	{
		return m_pWnd->get_accDescription(varChild, pszDescription);
	}

	STDMETHODIMP SAccessible::get_accRole(VARIANT varChild, VARIANT * pvarRole)
	{
		return m_pWnd->get_accRole(varChild, pvarRole);
	}

	STDMETHODIMP SAccessible::get_accState(VARIANT varChild, VARIANT * pvarState)
	{
		return m_pWnd->get_accState(varChild, pvarState);
	}

	STDMETHODIMP SAccessible::get_accHelp(VARIANT varChild, BSTR * pszHelp)
	{
		return m_pWnd->get_accHelp(varChild, pszHelp);
	}

	STDMETHODIMP SAccessible::get_accHelpTopic(BSTR * pszHelpFile, VARIANT varChild, long * pidTopic)
	{
		return m_pWnd->get_accHelpTopic(pszHelpFile,varChild, pidTopic);
	}

	STDMETHODIMP SAccessible::get_accKeyboardShortcut(VARIANT varChild, BSTR * pszKeyboardShortcut)
	{
		return m_pWnd->get_accKeyboardShortcut(varChild, pszKeyboardShortcut);
	}

	STDMETHODIMP SAccessible::get_accFocus(VARIANT * pvarChild)
	{
		return m_pWnd->get_accFocus(pvarChild);
	}

	STDMETHODIMP SAccessible::get_accSelection(VARIANT * pvarChildren)
	{
		return m_pWnd->get_accSelection(pvarChildren);
	}

	STDMETHODIMP SAccessible::get_accDefaultAction(VARIANT varChild, BSTR * pszDefaultAction)
	{
		return m_pWnd->get_accDefaultAction(varChild, pszDefaultAction);
	}

	STDMETHODIMP SAccessible::accSelect(long flagsSelect, VARIANT varChild)
	{
		return m_pWnd->accSelect(flagsSelect,varChild);
	}

	STDMETHODIMP SAccessible::accLocation(long * pxLeft, long * pyTop, long * pcxWidth, long * pcyHeight, VARIANT varChild)
	{
		return m_pWnd->accLocation(pxLeft,pyTop,pcxWidth,pcyHeight, varChild);
	}

	STDMETHODIMP SAccessible::accNavigate(long navDir, VARIANT varStart, VARIANT * pvarEndUpAt)
	{
		return m_pWnd->accNavigate(navDir,varStart,pvarEndUpAt);
	}

	STDMETHODIMP SAccessible::accHitTest(long xLeft, long yTop, VARIANT * pvarChild)
	{
		return m_pWnd->accHitTest(xLeft, yTop, pvarChild);
	}

	STDMETHODIMP SAccessible::accDoDefaultAction(VARIANT varChild)
	{
		return m_pWnd->accDoDefaultAction(varChild);
	}

	STDMETHODIMP SAccessible::put_accName(VARIANT varChild, BSTR szName)
	{
		return m_pWnd->put_accName(varChild, szName);
	}

	STDMETHODIMP SAccessible::put_accValue(VARIANT varChild, BSTR szValue)
	{
		return m_pWnd->put_accValue(varChild, szValue);
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

