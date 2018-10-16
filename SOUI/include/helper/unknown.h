#pragma once


#define IUNKNOWN_BEGIN2(iid,cls ) \
	STDMETHODIMP_(ULONG) AddRef(void)\
	{\
		return _AddRef();\
	}\
	STDMETHODIMP_(ULONG) Release(void)\
	{\
		return _Release();\
	}\
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj)\
	{\
		if (ppvObj == NULL)\
			return E_INVALIDARG;\
		*ppvObj = NULL;\
		if(IsEqualIID(riid,iid) || IsEqualIID(riid,IID_IUnknown))\
		{\
			*ppvObj=(cls *)this;\
		}\

#define IUNKNOWN_ADD_IID2(iid,cls) \
	if(IsEqualIID(riid,iid))\
	{\
		*ppvObj=(cls *)this;\
	}\

#define IUNKNOWN_END() \
	if (*ppvObj)\
	{\
		AddRef();\
		return S_OK;\
	}\
	return E_NOINTERFACE;\
	}

#define IUNKNOWN_BEGIN(iface ) \
IUNKNOWN_BEGIN2(__uuidof(iface),iface)

#define IUNKNOWN_ADD_IID(iface) \
IUNKNOWN_ADD_IID2(__uuidof(iface),iface)


class CUnknown
{
public:
	CUnknown():m_cRef(1)
	{

	}

	virtual ~CUnknown(){}

	virtual void OnFinalRelease(){}

protected:
	STDMETHODIMP_(ULONG) _AddRef(void)
	{
		return (ULONG)InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) _Release(void)
	{
		LONG ret = InterlockedDecrement(&m_cRef);
		if(ret == 0)
		{
			OnFinalRelease();
			delete this;
		}
		return ret;
	}

	LONG GetRef() const {
		return m_cRef;
	}
private:
	volatile LONG m_cRef;
};