#pragma once

#include "Render-Skia2-i.h"
#include "render-skia.h"
#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{
    class RenderTarget_Skia2 : public TObjRefImpl<IRenderTarget_Skia2>
    {
    public:
        RenderTarget_Skia2(void);
        virtual ~RenderTarget_Skia2(void);
        
        virtual HRESULT Init(IRenderTarget *pRT);
        virtual IRenderTarget* GetRenderTarget();
        
        virtual HRESULT translate(float dx, float dy);
        virtual HRESULT scale(float sx, float sy);
        virtual HRESULT rotate(float degrees);
        virtual HRESULT skew(float sx, float sy);

    protected:
        CAutoRefPtr<SRenderTarget_Skia> m_pRT;
    };
}
