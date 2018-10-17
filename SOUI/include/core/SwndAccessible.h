#pragma once
#include <oleacc.h>
#include <helper/unknown.h>
#include "Swnd.h"

namespace SOUI
{
#ifdef SOUI_ENABLE_ACC
	struct IAccProxy {
		virtual HRESULT get_accParent(IDispatch **ppdispParent) PURE;
		virtual HRESULT get_accChildCount(long *pcountChildren)PURE;
		virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild)PURE;
		virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName)PURE;
		virtual HRESULT get_accValue(VARIANT varChild, BSTR *pszValue)PURE;
		virtual HRESULT get_accDescription(VARIANT varChild, BSTR *pszDescription)PURE;
		virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole)PURE;
		virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState)PURE;
		virtual HRESULT get_accHelp(VARIANT varChild, BSTR *pszHelp)PURE;
		virtual HRESULT get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic)PURE;
		virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut)PURE;
		virtual HRESULT get_accFocus(VARIANT *pvarChild)PURE;
		virtual HRESULT get_accSelection(VARIANT *pvarChildren)PURE;
		virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)PURE;
		virtual HRESULT accSelect(long flagsSelect, VARIANT varChild)PURE;
		virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)PURE;
		virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt)PURE;
		virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild)PURE;
		virtual HRESULT accDoDefaultAction(VARIANT varChild)PURE;
		virtual HRESULT put_accName(VARIANT varChild, BSTR szName)PURE;
		virtual HRESULT put_accValue(VARIANT varChild, BSTR szValue)PURE;
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

#endif//SOUI_ENABLE_ACC
}
