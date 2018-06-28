/*
** Copyright 2008-2009, Ernest Laurentin (http://www.ernzo.com/)
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
**
** File:        AtlActiveXContainer.cpp
** Version:     1.1 - Support for windowless keyboard events
*/
#include "souistd.h"
#include "SAxContainer.h"


namespace SOUI
{

SAxContainer::SAxContainer()
: m_lRefCnt(1)
{
}

SAxContainer::~SAxContainer()
{
}

BOOL SAxContainer::CreateControl(REFGUID clsid ,DWORD dwClsCtx /*=CLSCTX_INPROC_SERVER*/)
{
    SASSERT(m_pAxHostDelegate);
    HRESULT hr = E_FAIL;
    SComPtr<IUnknown> pControl;
    hr = CoCreateInstance(clsid, NULL,dwClsCtx , __uuidof(IUnknown), reinterpret_cast<void**>(&pControl));
    if ( SUCCEEDED(hr) )
    {
        Init(pControl);
    }
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
// IUnknown
HRESULT SAxContainer::QueryInterface(REFIID iid, void** object)
{
    HRESULT hr = S_OK;
    *object = NULL;
    if (iid == IID_IUnknown) {
        // Avoid ambiguous resolution of IUnknown
        *object = static_cast<IUnknown*>(static_cast<MinimumIDispatchImpl*>(this));
    } else if (iid == IID_IDispatch) {
        *object = static_cast<MinimumIDispatchImpl*>(this);
    } else if (iid == IID_IOleContainer) {
        *object = static_cast<IOleContainer*>(this);
    } else if (iid == IID_IParseDisplayName) {
        *object = static_cast<IParseDisplayName*>(this);
    } else if (iid == IID_IBindHost) {
        *object = static_cast<IBindHost*>(this);
    } else if (iid == IID_IServiceProvider) {
        *object = static_cast<IServiceProvider*>(this);
    } else {
        hr=    ActiveXSite::QueryInterface2(iid,object);
    }
    if ( SUCCEEDED(hr) ) {
        static_cast<IUnknown*>(*object)->AddRef();
    }
    return hr;
}

ULONG STDMETHODCALLTYPE SAxContainer::AddRef()
{
    return ::InterlockedIncrement(&m_lRefCnt);
}

ULONG STDMETHODCALLTYPE SAxContainer::Release()
{
    // We don't destroy object when it reaches 0
    return ::InterlockedDecrement(&m_lRefCnt);
}


///////////////////////////////////////////////////////////////////////////////
// IBindHost
HRESULT SAxContainer::CreateMoniker(LPOLESTR szName, IBindCtx* /*pBC*/,
                         IMoniker** ppmk, DWORD /*dwReserved*/)
{
    HRESULT hr;
    typedef HRESULT (WINAPI* pfnCreateURLMonikerEx)(IMoniker *pMkCtx,LPCWSTR szURL,IMoniker **ppmk,DWORD dwFlags);
    HMODULE urlmon = ::GetModuleHandleA("urlmon.dll");
    if(!urlmon)
      urlmon = ::LoadLibraryA("urlmon.dll");
    pfnCreateURLMonikerEx OrigCreateURLMonikerEx = (pfnCreateURLMonikerEx)::GetProcAddress(urlmon,"CreateURLMonikerEx");
    hr = OrigCreateURLMonikerEx(NULL, szName, ppmk, URL_MK_UNIFORM);
    return hr;
}

HRESULT SAxContainer::MonikerBindToStorage(IMoniker *pMk, IBindCtx *pBC,
                                IBindStatusCallback *pBSC, REFIID riid, void **ppvObj)
{
    HRESULT hr = E_FAIL;
    SComPtr<IBindCtx> pBCCtx = pBC;
    if (pBCCtx == NULL)
    {
        // creates it if not present
        hr = CreateBindCtx(0, &pBCCtx);
    }

    if ( pBCCtx != NULL )
    {
        if ( pBSC != NULL )
        {
          typedef HRESULT (WINAPI* pfnRegisterBindStatusCallback)( IBindCtx *pbc,IBindStatusCallback *pbsc,IBindStatusCallback **ppbscPrevious,DWORD dwReserved);
          HMODULE urlmon = ::GetModuleHandleA("urlmon.dll");
          if(!urlmon)
            urlmon = ::LoadLibraryA("urlmon.dll");
          pfnRegisterBindStatusCallback OrigRegisterBindStatusCallback = (pfnRegisterBindStatusCallback)::GetProcAddress(urlmon,"RegisterBindStatusCallback");
          hr = OrigRegisterBindStatusCallback(pBCCtx, pBSC, NULL, 0);
        }
        hr = pMk->BindToStorage(pBCCtx, NULL, riid, ppvObj);
    }
    return hr;
}

HRESULT SAxContainer::MonikerBindToObject(IMoniker* /*pMk*/, IBindCtx* /*pBC*/,
                               IBindStatusCallback* /*pBSC*/, REFIID /*riid*/, void** /*ppvObj*/)
{
    ATLTRACENOTIMPL(_T("ActiveXContainerImpl::MonikerBindToObject\n"));
}

// {0002DF05-0000-0000-C000-000000000046}
static const GUID GUID_IWebBrowserApp = 
{ 0x0002DF05, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

// {1B36028E-B491-4BB2-8584-8A9E0A677D6E}
static const GUID GUID_IXcpControlHost = 
{ 0x1B36028E, 0xB491, 0x4BB2, { 0x85, 0x84, 0x8A, 0x9E, 0x0A, 0x67, 0x7D, 0x6E } };

HRESULT SAxContainer::QueryService( REFGUID guidService, REFIID riid, void **ppvObject )
{
    HRESULT hr = E_FAIL;
    if (guidService == IID_IBindHost ||
        guidService == GUID_IWebBrowserApp ||
        guidService == GUID_IXcpControlHost)
    {
        hr = QueryInterface(riid, ppvObject);
    }
    return hr;
}

}//end of namespace SOUI
