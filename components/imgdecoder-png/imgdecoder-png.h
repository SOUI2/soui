// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <unknown/obj-ref-impl.hpp>

#include <interface/imgdecoder-i.h>
#include <interface/render-i.h>

struct APNGDATA;

namespace SOUI
{
    
    class SImgFrame_PNG : public IImgFrame
    {
    public:
        SImgFrame_PNG();
        void Attach(const BYTE * pdata,int nWid,int nHei,int nDelay);

        virtual BOOL GetSize(UINT *pWid,UINT *pHei);
        virtual BOOL CopyPixels( 
            /* [unique][in] */ const RECT *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ BYTE *pbBuffer);
        virtual int GetDelay(){return m_nFrameDelay;}
    protected:
        int     m_nFrameDelay;
        int     m_nWid, m_nHei;
        const BYTE   *m_pdata;
    };
    
    class SImgX_PNG : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_PNG;
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
        SImgX_PNG(BOOL bPremultiplied);
        ~SImgX_PNG(void);
        
        int _DoDecode(APNGDATA *pData);

        BOOL m_bPremultiplied;
        
        APNGDATA       *    m_pngData;
        SImgFrame_PNG  *    m_pImgArray;
    };

    #define DESC_IMGDECODER L"apng"
    class SImgDecoderFactory_PNG : public TObjRefImpl<IImgDecoderFactory>
    {
    friend class SImgX_PNG;
    public:
        SImgDecoderFactory_PNG();
        ~SImgDecoderFactory_PNG();
        
        virtual HRESULT SaveImage(IBitmap *pImg, LPCWSTR pszFileName, const LPVOID pFormat);
        virtual BOOL CreateImgX(IImgX **ppImgDecoder);
        LPCWSTR GetDescription() const;
    };
    
    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_PNG
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }
}//end of namespace SOUI

