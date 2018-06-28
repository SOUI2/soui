#pragma once

#include <unknown/obj-ref-i.h>
#include <interface/render-i.h>
namespace SOUI
{
    struct __declspec( uuid("{D40F8B64-F383-48ae-A0F9-BBEB53ED5BF2}") ) IRenderTarget_Skia2 : public IObjRef
    {
        virtual HRESULT Init(IRenderTarget *pRT) =0;
        virtual IRenderTarget* GetRenderTarget() =0;
      
        virtual HRESULT translate(float dx, float dy) =0;
        virtual HRESULT scale(float sx, float sy)=0;
        virtual HRESULT rotate(float degrees)=0;
        virtual HRESULT skew(float sx, float sy)=0;
    };
}
