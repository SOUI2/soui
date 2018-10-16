#pragma once
#include <oleacc.h>
#include <helper/unknown.h>
#include "Swnd.h"

namespace SOUI
{
	class IAccProxy {
	public:
		virtual HRESULT get_accParent(IDispatch **ppdispParent) { return E_NOTIMPL; }
		virtual HRESULT get_accChildCount(long *pcountChildren){ return E_NOTIMPL; }
		virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild){ return E_NOTIMPL; }
		virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName){ return E_NOTIMPL; }
		virtual HRESULT get_accValue(VARIANT varChild, BSTR *pszValue){ return E_NOTIMPL; }
		virtual HRESULT get_accDescription(VARIANT varChild, BSTR *pszDescription){ return E_NOTIMPL; }
		virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole){ return E_NOTIMPL; }
		virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState){ return E_NOTIMPL; }
		virtual HRESULT get_accHelp(VARIANT varChild, BSTR *pszHelp){ return E_NOTIMPL; }
		virtual HRESULT get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic){ return E_NOTIMPL; }
		virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut){ return E_NOTIMPL; }
		virtual HRESULT get_accFocus(VARIANT *pvarChild){ return E_NOTIMPL; }
		virtual HRESULT get_accSelection(VARIANT *pvarChildren){ return E_NOTIMPL; }
		virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction){ return E_NOTIMPL; }
		virtual HRESULT accSelect(long flagsSelect, VARIANT varChild){ return E_NOTIMPL; }
		virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild){ return E_NOTIMPL; }
		virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt){ return E_NOTIMPL; }
		virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild){ return E_NOTIMPL; }
		virtual HRESULT accDoDefaultAction(VARIANT varChild){ return E_NOTIMPL; }
		virtual HRESULT put_accName(VARIANT varChild, BSTR szName){ return E_NOTIMPL; }
		virtual HRESULT put_accValue(VARIANT varChild, BSTR szValue){ return E_NOTIMPL; }
	};

	class SOUI_EXP SAccessible : public IAccessible , public SUnknown
	{
	protected:
		SWindow * m_pWnd;
	public:
		SAccessible(SWindow * pWnd);
		~SAccessible();
	public:
		// Implement IAccessible
		STDMETHODIMP get_accParent(IDispatch **ppdispParent);
		STDMETHODIMP get_accChildCount(long *pcountChildren);
		STDMETHODIMP get_accChild(VARIANT varChild, IDispatch **ppdispChild);
		STDMETHODIMP get_accName(VARIANT varChild, BSTR *pszName);
		STDMETHODIMP get_accValue(VARIANT varChild, BSTR *pszValue);
		STDMETHODIMP get_accDescription(VARIANT varChild, BSTR *pszDescription);
		STDMETHODIMP get_accRole(VARIANT varChild, VARIANT *pvarRole);
		STDMETHODIMP get_accState(VARIANT varChild, VARIANT *pvarState);
		STDMETHODIMP get_accHelp(VARIANT varChild, BSTR *pszHelp);
		STDMETHODIMP get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
		STDMETHODIMP get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut);
		STDMETHODIMP get_accFocus(VARIANT *pvarChild);
		STDMETHODIMP get_accSelection(VARIANT *pvarChildren);
		STDMETHODIMP get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
		STDMETHODIMP accSelect(long flagsSelect, VARIANT varChild);
		STDMETHODIMP accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
		STDMETHODIMP accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
		STDMETHODIMP accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
		STDMETHODIMP accDoDefaultAction(VARIANT varChild);
		STDMETHODIMP put_accName(VARIANT varChild, BSTR szName);
		STDMETHODIMP put_accValue(VARIANT varChild, BSTR szValue);

		// Implement IDispatch
		STDMETHODIMP GetTypeInfoCount(unsigned int FAR* pctinfo);
		STDMETHODIMP GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo);
		STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId);
		STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams,
			VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr);

	public:
		IUNKNOWN_BEGIN(IAccessible)
			IUNKNOWN_ADD_IID(IDispatch)
		IUNKNOWN_END()
	};


}
