// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the    
// Active Template Library product.

#ifndef __SCOMCLI_H__
#define __SCOMCLI_H__

#pragma once

#include <unknwn.h>

#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x);
#endif

#pragma warning (push)
#pragma warning (disable: 4127)  // conditional expression constant
#pragma warning (disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions


#pragma pack(push,8)
namespace SOUI
{

/////////////////////////////////////////////////////////////////////////////
// Smart Pointer helpers


inline IUnknown* SComPtrAssign(IUnknown** pp, IUnknown* lp)
{
    if (pp == NULL)
        return NULL;
        
    if (lp != NULL)
        lp->AddRef();
    if (*pp)
        (*pp)->Release();
    *pp = lp;
    return lp;
}

inline IUnknown* SComQIPtrAssign(IUnknown** pp, IUnknown* lp, REFIID riid)
{
    if (pp == NULL)
        return NULL;

    IUnknown* pTemp = *pp;
    *pp = NULL;
    if (lp != NULL)
        lp->QueryInterface(riid, (void**)pp);
    if (pTemp)
        pTemp->Release();
    return *pp;
}


/////////////////////////////////////////////////////////////////////////////
// COM Smart pointers

template <class T>
class _SNoAddRefReleaseOnCComPtr : public T
{
    private:
        STDMETHOD_(ULONG, AddRef)()=0;
        STDMETHOD_(ULONG, Release)()=0;
};

//SComPtrBase provides the basis for all other smart pointers
//The other smartpointers add their own constructors and operators
template <class T>
class SComPtrBase
{
protected:
    SComPtrBase() throw()
    {
        p = NULL;
    }
    SComPtrBase( int nNull) throw()
    {
        SASSERT(nNull == 0);
        (void)nNull;
        p = NULL;
    }
    SComPtrBase( T* lp) throw()
    {
        p = lp;
        if (p != NULL)
            p->AddRef();
    }
public:
    typedef T _PtrClass;
    ~SComPtrBase() throw()
    {
        if (p)
            p->Release();
    }
    operator T*() const throw()
    {
        return p;
    }
    T& operator*() const
    {
        SASSERT(p!=NULL);
        return *p;
    }
    //The assert on operator& usually indicates a bug.  If this is really
    //what is needed, however, take the address of the p member explicitly.
    T** operator&() throw()
    {
        SASSERT(p==NULL);
        return &p;
    }
    _SNoAddRefReleaseOnCComPtr<T>* operator->() const throw()
    {
        SASSERT(p!=NULL);
        return (_SNoAddRefReleaseOnCComPtr<T>*)p;
    }
    bool operator!() const throw()
    {
        return (p == NULL);
    }
    bool operator<( T* pT) const throw()
    {
        return p < pT;
    }
    bool operator!=( T* pT) const
    {
        return !operator==(pT);
    }
    bool operator==( T* pT) const throw()
    {
        return p == pT;
    }

    // Release the interface and set to NULL
    void Release() throw()
    {
        T* pTemp = p;
        if (pTemp)
        {
            p = NULL;
            pTemp->Release();
        }
    }
    // Compare two objects for equivalence
    bool IsEqualObject( IUnknown* pOther) throw()
    {
        if (p == NULL && pOther == NULL)
            return true;    // They are both NULL objects

        if (p == NULL || pOther == NULL)
            return false;    // One is NULL the other is not

        SComPtr<IUnknown> punk1;
        SComPtr<IUnknown> punk2;
        p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
        pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
        return punk1 == punk2;
    }
    // Attach to an existing interface (does not AddRef)
    void Attach( T* p2) throw()
    {
        if (p)
            p->Release();
        p = p2;
    }
    // Detach the interface (does not Release)
    T* Detach() throw()
    {
        T* pt = p;
        p = NULL;
        return pt;
    }
     HRESULT CopyTo( T** ppT) throw()
    {
        SASSERT(ppT != NULL);
        if (ppT == NULL)
            return E_POINTER;
        *ppT = p;
        if (p)
            p->AddRef();
        return S_OK;
    }
     HRESULT CoCreateInstance( REFCLSID rclsid,  LPUNKNOWN pUnkOuter = NULL,  DWORD dwClsContext = CLSCTX_ALL) throw()
    {
        SASSERT(p == NULL);
        return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
    }
     HRESULT CoCreateInstance( LPCOLESTR szProgID,  LPUNKNOWN pUnkOuter = NULL,  DWORD dwClsContext = CLSCTX_ALL) throw()
    {
        CLSID clsid;
        HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
        SASSERT(p == NULL);
        if (SUCCEEDED(hr))
            hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
        return hr;
    }
    template <class Q>
     HRESULT QueryInterface( Q** pp) const throw()
    {
        SASSERT(pp != NULL);
        return p->QueryInterface(__uuidof(Q), (void**)pp);
    }
    T* p;
};

template <class T>
class SComPtr : public SComPtrBase<T>
{
public:
    SComPtr() throw()
    {
    }
    SComPtr(int nNull) throw() :
        SComPtrBase<T>(nNull)
    {
    }
    SComPtr(T* lp) throw() :
        SComPtrBase<T>(lp)

    {
    }
    SComPtr( const SComPtr<T>& lp) throw() :
        SComPtrBase<T>(lp.p)
    {
    }
    T* operator=( T* lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<T*>(SComPtrAssign((IUnknown**)&this->p, lp));
        }
        return *this;
    }
    template <typename Q>
    T* operator=( const SComPtr<Q>& lp) throw()
    {
        if( !IsEqualObject(lp) )
        {
            return static_cast<T*>(SComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T)));
        }
        return *this;
    }
    T* operator=( const SComPtr<T>& lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<T*>(SComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
    }
};

//specialization for IDispatch
template <>
class SComPtr<IDispatch> : public SComPtrBase<IDispatch>
{
public:
    SComPtr() throw()
    {
    }
    SComPtr(IDispatch* lp) throw() :
        SComPtrBase<IDispatch>(lp)
    {
    }
    SComPtr(const SComPtr<IDispatch>& lp) throw() :
        SComPtrBase<IDispatch>(lp.p)
    {
    }
    IDispatch* operator=(IDispatch* lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<IDispatch*>(SComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
    }
    IDispatch* operator=(const SComPtr<IDispatch>& lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<IDispatch*>(SComPtrAssign((IUnknown**)&p, lp.p));
        }
        return *this;
    }

// IDispatch specific stuff
     HRESULT GetPropertyByName( LPCOLESTR lpsz,  VARIANT* pVar) throw()
    {
        SASSERT(p);
        SASSERT(pVar);
        DISPID dwDispID;
        HRESULT hr = GetIDOfName(lpsz, &dwDispID);
        if (SUCCEEDED(hr))
            hr = GetProperty(dwDispID, pVar);
        return hr;
    }
     HRESULT GetProperty( DISPID dwDispID,  VARIANT* pVar) throw()
    {
        return GetProperty(p, dwDispID, pVar);
    }
     HRESULT PutPropertyByName( LPCOLESTR lpsz,  VARIANT* pVar) throw()
    {
        SASSERT(p);
        SASSERT(pVar);
        DISPID dwDispID;
        HRESULT hr = GetIDOfName(lpsz, &dwDispID);
        if (SUCCEEDED(hr))
            hr = PutProperty(dwDispID, pVar);
        return hr;
    }
     HRESULT PutProperty( DISPID dwDispID,  VARIANT* pVar) throw()
    {
        return PutProperty(p, dwDispID, pVar);
    }
     HRESULT GetIDOfName( LPCOLESTR lpsz,  DISPID* pdispid) throw()
    {
        return p->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&lpsz), 1, LOCALE_USER_DEFAULT, pdispid);
    }
    // Invoke a method by DISPID with no parameters
     HRESULT Invoke0( DISPID dispid,  VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = { NULL, NULL, 0, 0};
        return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with no parameters
     HRESULT Invoke0( LPCOLESTR lpszName,  VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke0(dispid, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with a single parameter
     HRESULT Invoke1( DISPID dispid, VARIANT* pvarParam1,  VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = { pvarParam1, NULL, 1, 0};
        return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with a single parameter
     HRESULT Invoke1( LPCOLESTR lpszName, VARIANT* pvarParam1,  VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke1(dispid, pvarParam1, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with two parameters
     HRESULT Invoke2( DISPID dispid,  VARIANT* pvarParam1,  VARIANT* pvarParam2,  VARIANT* pvarRet = NULL) throw();
    // Invoke a method by name with two parameters
     HRESULT Invoke2( LPCOLESTR lpszName,  VARIANT* pvarParam1,  VARIANT* pvarParam2,  VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke2(dispid, pvarParam1, pvarParam2, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with N parameters
     HRESULT InvokeN(DISPID dispid, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = { pvarParams, NULL, (UINT)nParams, 0};
        return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with Nparameters
     HRESULT InvokeN(LPCOLESTR lpszName, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = InvokeN(dispid, pvarParams, nParams, pvarRet);
        return hr;
    }
     static HRESULT PutProperty( IDispatch* p,  DISPID dwDispID,  VARIANT* pVar) throw()
    {
        SASSERT(p);
        SASSERT(pVar != NULL);
        if (pVar == NULL)
            return E_POINTER;
        
        if(p == NULL)
            return E_INVALIDARG;
        
        DISPPARAMS dispparams = {NULL, NULL, 1, 1};
        dispparams.rgvarg = pVar;
        DISPID dispidPut = DISPID_PROPERTYPUT;
        dispparams.rgdispidNamedArgs = &dispidPut;

        if (pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH || 
            (pVar->vt & VT_ARRAY) || (pVar->vt & VT_BYREF))
        {
            HRESULT hr = p->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
                &dispparams, NULL, NULL, NULL);
            if (SUCCEEDED(hr))
                return hr;
        }
        return p->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
                &dispparams, NULL, NULL, NULL);
    }
     static HRESULT GetProperty( IDispatch* p,  DISPID dwDispID,  VARIANT* pVar) throw()
    {
        SASSERT(p);
        SASSERT(pVar != NULL);
        if (pVar == NULL)
            return E_POINTER;
        
        if(p == NULL)
            return E_INVALIDARG;
            
        DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
        return p->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
                &dispparamsNoArgs, pVar, NULL, NULL);
    }
};

template <class T, const IID* piid = &__uuidof(T)>
class SComQIPtr : public SComPtr<T>
{
public:
    SComQIPtr() throw()
    {
    }
    SComQIPtr( T* lp) throw() :
        SComPtr<T>(lp)
    {
    }
    SComQIPtr( const SComQIPtr<T,piid>& lp) throw() :
        SComPtr<T>(lp.p)
    {
    }
    SComQIPtr( IUnknown* lp) throw()
    {
        if (lp != NULL)
            lp->QueryInterface(*piid, (void **)&this->p);
    }
    T* operator=( T* lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<T*>(SComPtrAssign((IUnknown**)&this->p, lp));
        }
        return *this;
    }
    T* operator=( const SComQIPtr<T,piid>& lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<T*>(SComPtrAssign((IUnknown**)&this->p, lp.p));
        }
        return *this;
    }
    T* operator=( IUnknown* lp) throw()
    {
        if(*this!=lp)
        {
            return static_cast<T*>(SComQIPtrAssign((IUnknown**)&this->p, lp, *piid));
        }
        return *this;
    }
};

//Specialization to make it work
template<>
class SComQIPtr<IUnknown, &IID_IUnknown> : public SComPtr<IUnknown>
{
public:
    SComQIPtr() throw()
    {
    }
    SComQIPtr( IUnknown* lp) throw()
    {
        //Actually do a QI to get identity
        if (lp != NULL)
            lp->QueryInterface(__uuidof(IUnknown), (void **)&p);
    }
    SComQIPtr( const SComQIPtr<IUnknown,&IID_IUnknown>& lp) throw() :
        SComPtr<IUnknown>(lp.p)
    {
    }
    IUnknown* operator=( IUnknown* lp) throw()
    {
        if(*this!=lp)
        {
            //Actually do a QI to get identity
            return SComQIPtrAssign((IUnknown**)&p, lp, __uuidof(IUnknown));
        }
        return *this;
    }

    IUnknown* operator=( const SComQIPtr<IUnknown,&IID_IUnknown>& lp) throw()
    {
        if(*this!=lp)
        {
            return SComPtrAssign((IUnknown**)&p, lp.p);
        }
        return *this;
    }
};

typedef SComQIPtr<IDispatch, &__uuidof(IDispatch)> CComDispatchDriver;

}    // namespace SOUI
#pragma pack(pop)

#pragma warning (pop)    


#endif    // __SCOMCLI_H__
