// imgdecoder-gdip.cpp : Defines the exported functions for the DLL application.
//

#include <Windows.h>

#include <GdiPlus.h>
#pragma comment(lib,"gdiplus")

#include "imgdecoder-gdip.h"
#include <interface/render-i.h>

using namespace Gdiplus;

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    //  SImgFrame_PNG
    SImgFrame_GDIP::SImgFrame_GDIP()
        :m_pdata(NULL)
        ,m_nWid(0)
        ,m_nHei(0)
        ,m_nFrameDelay(0)
    {

    }


    SImgFrame_GDIP::~SImgFrame_GDIP()
    {
        if(m_pdata) delete []m_pdata;
    }

    void SImgFrame_GDIP::Attach( const BYTE * pdata,int nWid,int nHei,int nDelay )
    {
        int bufSize = nWid*nHei*4;
        m_pdata=new BYTE[bufSize];
        
        BYTE *pDst = m_pdata;
        const BYTE *pSrc = pdata;
        int pixel_count = nWid * nHei;
        for (int i=0; i < pixel_count; ++i) {
            BYTE a = pSrc[3];
            if (a) 
            {
                pDst[0] = (pSrc[0] *a)/255;
                pDst[1] = (pSrc[1] *a)/255;
                pDst[2] = (pSrc[2] *a)/255;
                pDst[3] = a;
            }else
            {
                memset(pDst,0,4);
            }
            pDst += 4;
            pSrc += 4;
        }

        m_nWid=(nWid);
        m_nHei=(nHei);
        m_nFrameDelay=(nDelay);
    }

    BOOL SImgFrame_GDIP::GetSize( UINT *pWid,UINT *pHei )
    {
        if(!m_pdata) return FALSE;
        *pWid = m_nWid;
        *pHei = m_nHei;
        return TRUE;
    }

    BOOL SImgFrame_GDIP::CopyPixels(const RECT *prc,UINT cbStride,  UINT cbBufferSize,  BYTE *pbBuffer )
    {
        if(!m_pdata || cbBufferSize != m_nHei * m_nWid *4) return FALSE;
        memcpy(pbBuffer,m_pdata,cbBufferSize);
        return TRUE;
    }


    //////////////////////////////////////////////////////////////////////////
    // SImgX_PNG
    int SImgX_GDIP::LoadFromMemory( void *pBuf,size_t bufLen )
    {
        HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, bufLen);
        BYTE* pMem = (BYTE*)::GlobalLock(hMem);
        memcpy(pMem,pBuf,bufLen);

        IStream* pStm = NULL;
        ::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
        
        Bitmap *  bmpSrc= new Bitmap(pStm);

        pStm->Release();
        ::GlobalUnlock(hMem);
        
        if (!bmpSrc) return 0;
        
        int nRet =  _InitFromGdipBitmap(bmpSrc);
        delete bmpSrc;
        
        return nRet;   
    }

    int SImgX_GDIP::LoadFromFile( LPCWSTR pszFileName )
    {
        Bitmap * bmpSrc= new Bitmap(pszFileName);    
        if (!bmpSrc) return 0;
        int nRet =  _InitFromGdipBitmap(bmpSrc);
        delete bmpSrc;
        return nRet;   
    }

    int SImgX_GDIP::LoadFromFile( LPCSTR pszFileName )
    {
        wchar_t wszFileName[MAX_PATH+1];
        MultiByteToWideChar(CP_ACP,0,pszFileName,-1,wszFileName,MAX_PATH);
        if(GetLastError()==ERROR_INSUFFICIENT_BUFFER) return 0;
        return LoadFromFile(wszFileName);
    }

    SImgX_GDIP::SImgX_GDIP( BOOL bPremultiplied )
        :m_bPremultiplied(bPremultiplied)
        ,m_pImgArray(NULL)
        ,m_nFrameCount(0)
    {

    }

    SImgX_GDIP::~SImgX_GDIP( void )
    {
        if(m_pImgArray) delete []m_pImgArray;
    }

    UINT SImgX_GDIP::GetFrameCount()
    {
        return m_nFrameCount;
    }

    int SImgX_GDIP::_InitFromGdipBitmap(Bitmap * bmpSrc)
    {
        GUID   pageGuid = FrameDimensionTime;
        // Get the number of frames in the first dimension.
        m_nFrameCount = max(1, bmpSrc->GetFrameCount(&pageGuid));
        
        SIZE imSize={bmpSrc->GetWidth(),bmpSrc->GetHeight()};

        if (m_nFrameCount>1)
        {
            m_pImgArray=new SImgFrame_GDIP[m_nFrameCount];
            int nSize = bmpSrc->GetPropertyItemSize(PropertyTagFrameDelay);
            // Allocate a buffer to receive the property item.
            PropertyItem* pDelays = (PropertyItem*) new char[nSize];
            bmpSrc->GetPropertyItem(PropertyTagFrameDelay, nSize, pDelays);
            Bitmap *bmp = new  Bitmap(imSize.cx,imSize.cy,PixelFormat32bppARGB);
            for (int i=0; i<m_nFrameCount; i++)
            {
                GUID pageGuid = FrameDimensionTime;
                bmpSrc->SelectActiveFrame(&pageGuid, i);
                
                
                Graphics g(bmp);
                g.Clear(Color(0,0,0,0));
                g.DrawImage(bmpSrc,0,0,imSize.cx,imSize.cy);
                
                int nFrameDelay =10*max(((int*) pDelays->value)[i], 10);
                
                BitmapData* bitmapData = new BitmapData;
                Rect rect(0,0, imSize.cx,imSize.cy);

                bmp->LockBits(
                    &rect,
                    ImageLockModeRead,
                    PixelFormat32bppARGB,
                    bitmapData);
                m_pImgArray[i].Attach((BYTE*)bitmapData->Scan0,imSize.cx,imSize.cy,nFrameDelay);

                bmp->UnlockBits(bitmapData);

                delete bitmapData;
            }
            delete bmp; 
            delete [] pDelays;
        }
        else
        {
            m_pImgArray = new SImgFrame_GDIP[1];

            BitmapData* bitmapData = new BitmapData;
            Rect rect(0,0, imSize.cx,imSize.cy);

            bmpSrc->LockBits(
                &rect,
                ImageLockModeRead,
                PixelFormat32bppARGB,
                bitmapData);

            m_pImgArray[0].Attach((BYTE*)bitmapData->Scan0,imSize.cx,imSize.cy,0);
            
            bmpSrc->UnlockBits(bitmapData);
            
            delete bitmapData;
        }
        
        return m_nFrameCount;
    }

    //////////////////////////////////////////////////////////////////////////
    //  SImgDecoderFactory_PNG

    SImgDecoderFactory_GDIP::SImgDecoderFactory_GDIP( )
    {
        GdiplusStartupInput gdiplusStartupInput;

        BOOL bOK = Ok == GdiplusStartup(&_gdiPlusToken, &gdiplusStartupInput, NULL);
        if(!bOK)
        {
            SASSERT(0);
        }
    }

    SImgDecoderFactory_GDIP::~SImgDecoderFactory_GDIP()
    {
        if (_gdiPlusToken != 0)
        {
            GdiplusShutdown(_gdiPlusToken);
            _gdiPlusToken = 0;
        }
    }

    BOOL SImgDecoderFactory_GDIP::CreateImgX( IImgX **ppImgDecoder )
    {
        *ppImgDecoder = new SImgX_GDIP(TRUE);
        return TRUE;
    }

    CLSID FindCodecForFileType( REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, UINT nCodecs )
    {
        for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
        {
            if( pCodecs[iCodec].FormatID == guidFileType )
            {
                return( pCodecs[iCodec].Clsid );
            }
        }

        return( CLSID_NULL );
    }

    HRESULT SImgDecoderFactory_GDIP::SaveImage(IBitmap *pImg, LPCWSTR pszFileName,const LPVOID pFormat)
    {
        const GUID * pFmtID = (const GUID*)pFormat;
        
        UINT nEncoders;
        UINT nBytes;
        Gdiplus::Status status;

        status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
        if( status != Gdiplus::Ok )
        {
            return( E_FAIL );
        }

        //USES_ATL_SAFE_ALLOCA;
        Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( malloc(nBytes) );

        if( pEncoders == NULL )
            return E_OUTOFMEMORY;

        status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
        if( status != Gdiplus::Ok )
        {
            free(pEncoders);
            return( E_FAIL );
        }

        CLSID clsidEncoder = FindCodecForFileType( *pFmtID, pEncoders, nEncoders );
        free(pEncoders);

        if( clsidEncoder == CLSID_NULL )
        {
            return( E_FAIL );
        }
        
        LPVOID pBits = pImg->LockPixelBits();
        Bitmap bmp(pImg->Width(),pImg->Height(),pImg->Width()*4,PixelFormat32bppPARGB,(BYTE*)pBits);
        pImg->UnlockPixelBits(pBits);
        Image *gdipImg = &bmp;
        return Ok == gdipImg->Save(pszFileName,&clsidEncoder)?S_OK:E_FAIL;
    }

    LPCWSTR SImgDecoderFactory_GDIP::GetDescription() const
    {
        return DESC_IMGDECODER;
    }

    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_GDIP
    {
        BOOL SCreateInstance( IObjRef **pImgDecoderFactory )
        {
            *pImgDecoderFactory = new SImgDecoderFactory_GDIP();
            return TRUE;
        }
    }

}//end of namespace SOUI

