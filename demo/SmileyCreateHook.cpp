#include "stdafx.h"
#include "SmileyCreateHook.h"
#include "mhook.h"

typedef HRESULT (STDAPICALLTYPE *_CoCreateInstance)(
    __in     REFCLSID rclsid,
    __in_opt LPUNKNOWN pUnkOuter,
    __in     DWORD dwClsContext, 
    __in     REFIID riid, 
    __deref_out LPVOID FAR* ppv);


_CoCreateInstance TrueCoCreateInstance = (_CoCreateInstance)GetProcAddress(GetModuleHandle(_T("ole32")),"CoCreateInstance");



HRESULT STDAPICALLTYPE HookCoCreateInstance(
    __in     REFCLSID rclsid,
    __in_opt LPUNKNOWN pUnkOuter,
    __in     DWORD dwClsContext, 
    __in     REFIID riid, 
    __deref_out LPVOID FAR* ppv)
{
    if(rclsid == CLSID_SSmileyCtrl)
    {
        return CreateSmiley(riid,ppv);
    }
    return TrueCoCreateInstance(rclsid,pUnkOuter,dwClsContext,riid,ppv);
}

typedef HRESULT (STDAPICALLTYPE * _ProgIDFromCLSID) (__in REFCLSID clsid, __deref_out LPOLESTR FAR* lplpszProgID);

_ProgIDFromCLSID TrueProgIDFromCLSID = (_ProgIDFromCLSID)GetProcAddress(GetModuleHandle(_T("ole32")),"ProgIDFromCLSID");

HRESULT STDAPICALLTYPE HookProgIDFromCLSID (__in REFCLSID clsid, __deref_out LPOLESTR FAR* lplpszProgID)
{
    if(clsid == CLSID_SSmileyCtrl)
    {
        const WCHAR KProgID[] = L"SSmileyCtrl.NoCOM.1";
        *lplpszProgID = (LPOLESTR)CoTaskMemAlloc(sizeof(KProgID));
        wcscpy(*lplpszProgID,KProgID);
        return S_OK;
    }
    return TrueProgIDFromCLSID(clsid,lplpszProgID);
}


SmileyCreateHook::SmileyCreateHook(void)
{
    if (!Mhook_SetHook((PVOID*)&TrueCoCreateInstance, HookCoCreateInstance))
    {
        SASSERT(FALSE);
    }

    if (!Mhook_SetHook((PVOID*)&TrueProgIDFromCLSID, HookProgIDFromCLSID))
    {
        SASSERT(FALSE);
    }
}

SmileyCreateHook::~SmileyCreateHook(void)
{
    Mhook_Unhook((PVOID*)&TrueProgIDFromCLSID);
    Mhook_Unhook((PVOID*)&TrueCoCreateInstance);

}
