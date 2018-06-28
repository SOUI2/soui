// imgdecoder-wic.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "imgdecoder-wic.h"

#pragma  comment(lib,"windowscodecs.lib")

namespace SOUI
{


    //////////////////////////////////////////////////////////////////////////
    // SImgFrame_WIC
    SImgFrame_WIC::SImgFrame_WIC( IWICBitmapSource *pFrame ):m_pFrame(pFrame),m_nFrameDelay(0)
    {

    }
    
    void SImgFrame_WIC::SetWICBitmapSource( IWICBitmapSource *pFrame )
    {
        m_pFrame=pFrame;
    }

    void SImgFrame_WIC::SetFrameDelay( int nDelay )
    {
        m_nFrameDelay=nDelay;
    }

    BOOL SImgFrame_WIC::GetSize( UINT *pWid,UINT *pHei )
    {
        SASSERT(m_pFrame);
        return S_OK == m_pFrame->GetSize(pWid,pHei);
    }

    BOOL SImgFrame_WIC::CopyPixels(const RECT *prc, UINT cbStride, UINT cbBufferSize, BYTE *pbBuffer )
    {
        SASSERT(m_pFrame);
        if(!prc)
        {
            return S_OK==m_pFrame->CopyPixels(NULL,cbStride,cbBufferSize,pbBuffer);
        }
        else
        {
            WICRect rc={prc->left,prc->top,prc->right-prc->left,prc->bottom-prc->top};
            return S_OK==m_pFrame->CopyPixels(&rc,cbStride,cbBufferSize,pbBuffer);
        }
    }



    //////////////////////////////////////////////////////////////////////////

    SImgX_WIC::SImgX_WIC(BOOL bPremultiplied)
    :m_pImgArray(NULL)
    ,m_uImgCount(0)
    ,m_bPremultiplied(bPremultiplied)
    {
    }

    SImgX_WIC::~SImgX_WIC(void)
    {
        if(m_pImgArray) delete []m_pImgArray;
        m_pImgArray = NULL;
    }

    int SImgX_WIC::LoadFromMemory(void *pBuf,size_t bufLen )
    {
        SASSERT(m_pImgArray == NULL);

        IWICImagingFactory*    factory    = SImgDecoderFactory_WIC::s_wicImgFactory;
        CAutoRefPtr<IWICBitmapDecoder>     decoder;
        CAutoRefPtr<IWICStream> stream ;

        if(FAILED(factory->CreateStream(&stream))) return 0;

        if(FAILED(stream->InitializeFromMemory((BYTE*)pBuf,(DWORD)bufLen))) return 0;

        if(FAILED(factory->CreateDecoderFromStream(stream,NULL,WICDecodeMetadataCacheOnDemand,&decoder))) return 0;

        return _DoDecode(decoder);
    }

    int SImgX_WIC::LoadFromFile( LPCWSTR pszFileName )
    {
        SASSERT(m_pImgArray == NULL);
        IWICImagingFactory*    factory    = SImgDecoderFactory_WIC::s_wicImgFactory;

        CAutoRefPtr<IWICBitmapDecoder>     decoder;

        HRESULT hr = factory->CreateDecoderFromFilename(
            pszFileName,
            NULL,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder
            );

        if(FAILED(hr)) return 0;

        return _DoDecode(decoder);
    }

    int SImgX_WIC::LoadFromFile( LPCSTR pszFileName )
    {
        wchar_t wszFileName[MAX_PATH+1];
        MultiByteToWideChar(CP_ACP,0,pszFileName,-1,wszFileName,MAX_PATH);
        if(GetLastError()==ERROR_INSUFFICIENT_BUFFER) return 0;
        return LoadFromFile(wszFileName);
    }

    int SImgX_WIC::_DoDecode( IWICBitmapDecoder * pDecoder )
    {
        SASSERT(m_uImgCount == 0);
        
        IWICImagingFactory*    factory    = SImgDecoderFactory_WIC::s_wicImgFactory;
        CAutoRefPtr<IWICFormatConverter> converter;
        if(FAILED(factory->CreateFormatConverter(&converter))) 
            return 0;

        if(FAILED(pDecoder->GetFrameCount(&m_uImgCount)))  
            return 0;

        m_pImgArray = new SImgFrame_WIC[m_uImgCount];
        for(UINT i = 0; i< m_uImgCount ;i++)
        {
            CAutoRefPtr<IWICBitmapFrameDecode> frame;
            if(SUCCEEDED(pDecoder->GetFrame(i,&frame)))
            {
//                 CAutoRefPtr<IWICMetadataQueryReader> pMetaReader;
//                 pDecoder->GetMetadataQueryReader(&pMetaReader);
//                 if(pMetaReader)
//                 {
//                     PROPVARIANT propValue;
//                     PropVariantInit(&propValue);
//                     HRESULT hr=pMetaReader->GetMetadataByName(L"/grctlext/Delay", &propValue);
//                     if(SUCCEEDED(hr))
//                     {
//                         m_pImgArray[i].SetFrameDelay(propValue.intVal);
//                     }
//                     PropVariantClear(&propValue);
//                 }
                converter->Initialize(frame,
                    m_bPremultiplied?GUID_WICPixelFormat32bppPBGRA:GUID_WICPixelFormat32bppBGRA,
                    WICBitmapDitherTypeNone,NULL,
                    0.f,WICBitmapPaletteTypeCustom);
                CAutoRefPtr<IWICBitmapSource> bmp;
                converter->QueryInterface(IID_PPV_ARGS(&bmp));
                m_pImgArray[i].SetWICBitmapSource(bmp);
            }
        }
        return m_uImgCount;
    }

    //////////////////////////////////////////////////////////////////////////
    CAutoRefPtr<IWICImagingFactory> SImgDecoderFactory_WIC::s_wicImgFactory;

    SImgDecoderFactory_WIC::SImgDecoderFactory_WIC()
    {
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,NULL,
            CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&s_wicImgFactory));
        (hr);
        SASSERT(SUCCEEDED(hr));
    }

    SImgDecoderFactory_WIC::~SImgDecoderFactory_WIC()
    {
        if(s_wicImgFactory) s_wicImgFactory = NULL;
    }

    BOOL SImgDecoderFactory_WIC::CreateImgX(IImgX ** ppImgDecoder)
    {
        *ppImgDecoder = new SImgX_WIC(TRUE);
        return TRUE;
    }
    
    HRESULT SImgDecoderFactory_WIC::SaveImage(IBitmap *pImg, LPCWSTR pszFileName, const LPVOID pFormat)
    {
        return E_FAIL;
    }

    LPCWSTR SImgDecoderFactory_WIC::GetDescription() const
    {
        return DESC_IMGDECODER;
    }

    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_WIC
    {
        BOOL SCreateInstance(IObjRef **pImgDecoderFactory)
        {
            *pImgDecoderFactory = new SImgDecoderFactory_WIC();
            return TRUE;
        }
    }

}//end of namespace SOUI

