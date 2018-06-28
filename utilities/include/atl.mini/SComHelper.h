#pragma once

#include <Unknwn.h>

namespace SOUI
{
    #define COM_INTERFACE_BEGIN() \
    public:\
    virtual HRESULT STDMETHODCALLTYPE QueryInterface2( \
        /* [in] */ REFIID riid,\
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)\
        {\
            HRESULT hr = E_NOINTERFACE; \
            
    #define COM_INTERFACE(IFACE)    \
            if(hr != S_OK && riid == __uuidof(IFACE)) \
            {\
                *ppvObject = static_cast<IFACE*>(this); \
                hr = S_OK;\
            }\
                
    #define COM_INTERFACE_END()  \
            return hr; \
         }
    
     
    template<class T>
    class SUnknownImpl: public T
    {
    public:
        SUnknownImpl():m_cRef(1){}

        template<typename P1>
        SUnknownImpl(P1 p1):T(p1),m_cRef(1)
        {
        }

        template<typename P1,typename P2>
        SUnknownImpl(P1 p1,P2 p2):T(p1,p2),m_cRef(1)
        {
        }

        virtual ~SUnknownImpl(){}
        
        virtual HRESULT STDMETHODCALLTYPE QueryInterface2( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
        {
			(riid);
			(ppvObject);
            return E_NOINTERFACE;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
        {
            HRESULT hr = E_NOINTERFACE;
            if(riid == __uuidof(IUnknown))
            {
                *ppvObject = this;
                hr =  S_OK;
            }else
            {
                hr = QueryInterface2(riid,ppvObject);
            }
            if(hr == S_OK)
            {
                static_cast<IUnknown*>(*ppvObject)->AddRef();
            }
            return hr;
        }

        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return (ULONG)::InterlockedIncrement(&m_cRef);
        }

        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            LONG uRet = ::InterlockedDecrement(&m_cRef);
            if(uRet == 0)
            {
                OnFinialRelease();
            }
            return uRet;
        }

    public:
    
        virtual void OnFinialRelease(){delete this;}

    protected:
        volatile LONG m_cRef;
    };
}
