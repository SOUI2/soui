#pragma once
#include <oleacc.h>
#include <helper/unknown.h>
#include "Swnd.h"

namespace SOUI
{
#define IMPLACCPROXY2(cls) \
	IMPLACCPROXY((cls::GetClassName()))

#define IMPLACCPROXY(acc_class) \
		SAccProxyWindow * CreateAccProxy() {\
			SObjectInfo objInfo(acc_class, AccProxy); \
			SAccProxyWindow * pAccProxy = (SAccProxyWindow *)SApplication::getSingletonPtr()->CreateObject(objInfo); \
			SASSERT(pAccProxy); \
			pAccProxy->Init(this);\
			return pAccProxy;\
		}
		


#ifdef SOUI_ENABLE_ACC
	
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

	class SOUI_EXP SAccessible : public IAccessible , public SUnknown
	{
	protected:
		SWindow * m_pWnd;
	public:
		SAccessible(SWindow * pWnd);
		~SAccessible();

		void SetOwner(SWindow *pWnd){m_pWnd = pWnd;}
		SWindow * GetOwner(){return m_pWnd;}
	protected:
		BOOL accValidateNavStart(VARIANT * pvar) const;
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
