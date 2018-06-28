#include "Render-Skia2.h"

namespace SOUI
{
    RenderTarget_Skia2::RenderTarget_Skia2(void)
    {
    }

    RenderTarget_Skia2::~RenderTarget_Skia2(void)
    {
    }

    HRESULT RenderTarget_Skia2::Init( IRenderTarget *pRT )
    {
        m_pRT = (SRenderTarget_Skia*)pRT;
        return S_OK;
    }

    IRenderTarget* RenderTarget_Skia2::GetRenderTarget()
    {
        return m_pRT;
    }

    HRESULT RenderTarget_Skia2::translate( float dx, float dy )
    {
        if(!m_pRT) return S_FALSE;
        m_pRT->GetCanvas()->translate(dx,dy);
        return S_OK;
    }

    HRESULT RenderTarget_Skia2::scale( float sx, float sy )
    {
        if(!m_pRT) return S_FALSE;
        m_pRT->GetCanvas()->scale(sx,sy);
        return S_OK;
    }

    HRESULT RenderTarget_Skia2::rotate( float degrees )
    {
        if(!m_pRT) return S_FALSE;
        m_pRT->GetCanvas()->rotate(degrees);
        return S_OK;
    }

    HRESULT RenderTarget_Skia2::skew( float sx, float sy )
    {
        if(!m_pRT) return S_FALSE;
        m_pRT->GetCanvas()->skew(sx,sy);
        return S_OK;
    }

}
