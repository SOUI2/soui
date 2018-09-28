#include "StdAfx.h"
#include "SSkinAPNG.h"
#include <helper/SplitString.h>
#include <interface/imgdecoder-i.h>
#include <interface/render-i.h>


namespace SOUI
{

    HRESULT SSkinAPNG::OnAttrSrc( const SStringW &strValue,BOOL bLoading )
    {
        SStringTList strLst;
        size_t nSegs=ParseResID(S_CW2T(strValue),strLst);
        LPBYTE pBuf=NULL;
        size_t szBuf=0;

        if(nSegs == 2)
        {
            szBuf=GETRESPROVIDER->GetRawBufferSize(strLst[0],strLst[1]);
            if(szBuf)
            {
                pBuf=new BYTE[szBuf];
                GETRESPROVIDER->GetRawBuffer(strLst[0],strLst[1],pBuf,szBuf);
            }
        }else
        {//自动从APNG资源类型里查找资源
            szBuf=GETRESPROVIDER->GetRawBufferSize(_T("apng"),strLst[0]);
            if(szBuf)
            {
                pBuf=new BYTE[szBuf];
                GETRESPROVIDER->GetRawBuffer(_T("apng"),strLst[0],pBuf,szBuf);
            }
        }
        if(pBuf)
        {
            LoadFromMemory(pBuf,szBuf);
            delete []pBuf;
        }
        return S_OK;
    }



int SSkinAPNG::LoadFromFile( LPCTSTR pszFileName )
{
    CAutoRefPtr<IImgX> imgX;
    GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&imgX);
    imgX->LoadFromFile(pszFileName);
    return _InitImgFrame(imgX);
}

int SSkinAPNG::LoadFromMemory( LPVOID pBuf,size_t dwSize )
{
    CAutoRefPtr<IImgX> imgX;
    GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&imgX);
    imgX->LoadFromMemory(pBuf,dwSize);
    return _InitImgFrame(imgX);
}

int SSkinAPNG::_InitImgFrame( IImgX *pImgX )
{
    if(m_pFrames) delete []m_pFrames;
    m_pFrames = NULL;
    m_nFrames =0;
    m_iFrame = 0;
    if(!pImgX) return 0;

    m_nFrames = pImgX->GetFrameCount();
    m_pFrames = new SAniImgFrame[m_nFrames];
    for(int i=0;i<m_nFrames;i++)
    {
        GETRENDERFACTORY->CreateBitmap(&m_pFrames[i].pBmp);
        m_pFrames[i].pBmp->Init(pImgX->GetFrame(i));
        m_pFrames[i].nDelay=pImgX->GetFrame(i)->GetDelay();
    }
    return m_nFrames;
}

}//end of namespace SOUI