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
** File:        AtlAxUtil.cpp
** Version:     1.0
*/
#ifndef ATLAXUTIL_HPP
#define ATLAXUTIL_HPP

/**
 * NoRefIUnknownImpl class
 */
template <class Base> class NoRefIUnknownImpl : public Base
{
    public:
    ~NoRefIUnknownImpl() {
      Base::FinalRelease();
    }

    // IUnknown
    virtual ULONG STDMETHODCALLTYPE AddRef() { return 1; }
    virtual ULONG STDMETHODCALLTYPE Release() { return 0; }
};

/**
 * MinimumIDispatchImpl
 */
class MinimumIDispatchImpl : public IDispatch {
    public:
    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
    {
        *pctinfo = 0;
        return S_OK;
    }
    STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo** /*ppTInfo*/)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(GetIDsOfNames)(REFIID /*riid*/, LPOLESTR* /*rgszNames*/,
                             UINT /*cNames*/, LCID /*lcid*/, DISPID* /*rgDispId*/)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(Invoke)(DISPID /*dispIdMember*/, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/,
                      DISPPARAMS* /*pDispParams*/, VARIANT* /*pVarResult*/, EXCEPINFO* /*pExcepInfo*/,
                      UINT* /*puArgErr*/)
    {
        // check 'OleCtl.h' for list of DISPID_*
        return E_NOTIMPL;
    }
};

#endif //ATLAXUTIL_HPP