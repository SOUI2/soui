// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <unknown/obj-ref-impl.hpp>

#include <interface/imgdecoder-i.h>

namespace SOUI
{
    
    class SImgFrame_STB : public IImgFrame
    {
    public:
        SImgFrame_STB(BYTE *data,int w,int h);
        ~SImgFrame_STB();

        virtual BOOL GetSize(UINT *pWid,UINT *pHei);
        virtual BOOL CopyPixels( 
            /* [unique][in] */ const RECT *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ BYTE *pbBuffer);
        virtual int GetDelay(){return 0;}

    protected:
        unsigned char * m_data;
        int      m_nWid,m_nHei;
    };
    
    class SImgX_STB : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_STB;
    public:

        int LoadFromMemory(void *pBuf,size_t bufLen);
        int LoadFromFile(LPCWSTR pszFileName);
        int LoadFromFile(LPCSTR pszFileName);

        IImgFrame * GetFrame(UINT iFrame){
            if(iFrame >= GetFrameCount()) return NULL;
            return m_pImg;
        }
        virtual UINT GetFrameCount(){return m_pImg?1:0;}
    protected:
        SImgX_STB(BOOL bPremultiple);
        ~SImgX_STB(void);
        
        void _DoPromultiply(BYTE *pdata,int nWid,int nHei);

        BOOL m_bPremultiple;
        SImgFrame_STB *     m_pImg;
    };

    #define DESC_IMGDECODER L"stb"
    class SImgDecoderFactory_STB : public TObjRefImpl<IImgDecoderFactory>
    {
    friend class SImgX_STB;
    public:
        SImgDecoderFactory_STB();
        ~SImgDecoderFactory_STB();
        
        virtual BOOL CreateImgX(IImgX **ppImgDecoder);
        LPCWSTR GetDescription() const;
    };
    
    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_STB
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }
}//end of namespace SOUI

