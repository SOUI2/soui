// imgdecoder-gdip.h : Defines the exported functions for the DLL application.
//


#include <unknown/obj-ref-impl.hpp>
#include <interface/imgdecoder-i.h>

namespace Gdiplus
{
    class Bitmap;
}

namespace SOUI
{

    class SImgFrame_GDIP : public IImgFrame
    {
    public:
        SImgFrame_GDIP();
        ~SImgFrame_GDIP();
        
        void Attach(const BYTE * pdata,int nWid,int nHei,int nDelay);

        virtual BOOL GetSize(UINT *pWid,UINT *pHei);
        virtual BOOL CopyPixels( 
             const RECT *prc,
             UINT cbStride,
             UINT cbBufferSize,
             BYTE *pbBuffer);
        virtual int GetDelay(){return m_nFrameDelay;}
    protected:
        int     m_nFrameDelay;
        int     m_nWid, m_nHei;
        BYTE   *m_pdata;
    };

    class SImgX_GDIP : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_GDIP;
    public:

        int LoadFromMemory(void *pBuf,size_t bufLen);
        int LoadFromFile(LPCWSTR pszFileName);
        int LoadFromFile(LPCSTR pszFileName);

        IImgFrame * GetFrame(UINT iFrame){
            if(iFrame >= GetFrameCount()) return NULL;
            return m_pImgArray+iFrame;
        }
        virtual UINT GetFrameCount();
    protected:
        SImgX_GDIP(BOOL bPremultiplied);
        ~SImgX_GDIP(void);
        
        int _InitFromGdipBitmap(Gdiplus::Bitmap * pSrcBmp);
        
        BOOL m_bPremultiplied;
        
        int                  m_nFrameCount;
        SImgFrame_GDIP  *    m_pImgArray;
    };

    #define DESC_IMGDECODER L"gdi+"
    class SImgDecoderFactory_GDIP : public TObjRefImpl<IImgDecoderFactory>
    {
        friend class SImgX_GDIP;
    public:
        SImgDecoderFactory_GDIP();
        ~SImgDecoderFactory_GDIP();

        virtual BOOL CreateImgX(IImgX **ppImgDecoder);
        virtual HRESULT SaveImage(IBitmap *pImg, LPCWSTR pszFileName, const LPVOID pFormat);
        virtual LPCWSTR GetDescription() const;
    protected:
    
        
        ULONG_PTR _gdiPlusToken;
    };

    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_GDIP
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }
}//end of namespace SOUI

