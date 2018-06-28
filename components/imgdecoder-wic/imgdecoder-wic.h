// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <wincodec.h>
#include <unknown/obj-ref-impl.hpp>

#include <interface/imgdecoder-i.h>

namespace SOUI
{
    
    class SImgFrame_WIC : public IImgFrame
    {
    public:
        SImgFrame_WIC(IWICBitmapSource *pFrame=NULL);
        void SetWICBitmapSource(IWICBitmapSource *pFrame);
        void SetFrameDelay(int nDelay);

        virtual BOOL GetSize(UINT *pWid,UINT *pHei);
        virtual BOOL CopyPixels( 
            /* [unique][in] */ const RECT *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ BYTE *pbBuffer);
        virtual int GetDelay(){return m_nFrameDelay;}
    protected:
        CAutoRefPtr<IWICBitmapSource>  m_pFrame;
        int     m_nFrameDelay;
    };
    
    class SImgX_WIC : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_WIC;
    public:

        int LoadFromMemory(void *pBuf,size_t bufLen);
        int LoadFromFile(LPCWSTR pszFileName);
        int LoadFromFile(LPCSTR pszFileName);

        IImgFrame * GetFrame(UINT iFrame){
            if(iFrame >= m_uImgCount) return NULL;
            return m_pImgArray+iFrame;
        }
        virtual UINT GetFrameCount(){return m_uImgCount;}
    protected:
        SImgX_WIC(BOOL bPremultiplied);
        ~SImgX_WIC(void);
        
        int _DoDecode(IWICBitmapDecoder * pDecoder);

        SImgFrame_WIC *     m_pImgArray;
        UINT				m_uImgCount;
        BOOL                m_bPremultiplied;

    };

    #define DESC_IMGDECODER L"wic"
    class SImgDecoderFactory_WIC : public TObjRefImpl<IImgDecoderFactory>
    {
    friend class SImgX_WIC;
    public:
        SImgDecoderFactory_WIC();
        ~SImgDecoderFactory_WIC();
        
        virtual BOOL CreateImgX(IImgX **ppImgDecoder);
        HRESULT SaveImage(IBitmap *pImg, LPCWSTR pszFileName, const LPVOID pFormat);
        LPCWSTR GetDescription() const;
    protected:
        static CAutoRefPtr<IWICImagingFactory> s_wicImgFactory;
    };
    
    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_WIC
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }
}//end of namespace SOUI

