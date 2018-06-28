#include "string/strcpcvt.h"

namespace SOUI
{

SStringW SStrCpCvt::CvtW2W( const SStringW &str,unsigned int)
{
    return str;
}

SStringA SStrCpCvt::CvtA2A( const SStringA & str,unsigned int cpFrom/*=CP_UTF8*/,unsigned int cpTo/*=CP_ACP*/ )
{
    if(cpTo==cpFrom)
        return str;
    SStringW strw=CvtA2W(str,cpFrom);
    return CvtW2A(strw,cpTo);
}

SStringW SStrCpCvt::CvtA2W( const SStringA & str,unsigned int cp/*=CP_ACP*/,unsigned int cp2/*=0*/ )
{
    UNREFERENCED_PARAMETER(cp2);
    wchar_t szBuf[1024];
    int nRet=MultiByteToWideChar(cp,0,str,str.GetLength(),szBuf,1024);
    if(nRet>0)
    {
        return SStringW(szBuf,nRet);
    }
    if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
    {
        int nRet=MultiByteToWideChar(cp,0,str,str.GetLength(),NULL,0);
        if(nRet>0)
        {
            wchar_t *pBuf=new wchar_t[nRet];
            MultiByteToWideChar(cp,0,str,str.GetLength(),pBuf,nRet);
            SStringW strRet(pBuf,nRet);
            delete []pBuf;
            return strRet;
        }
    }
    return L"";
}

SStringA SStrCpCvt::CvtW2A( const SStringW & str,unsigned int cp/*=CP_ACP*/ )
{
    char szBuf[1024];
    int nRet=WideCharToMultiByte(cp,0,str,str.GetLength(),szBuf,1024,NULL,NULL);
    if(nRet>0) return SStringA(szBuf,nRet);
    if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
    {
        int nRet=WideCharToMultiByte(cp,0,str,str.GetLength(),NULL,0,NULL,NULL);
        if(nRet>0)
        {
            char *pBuf=new char[nRet];
            WideCharToMultiByte(cp,0,str,str.GetLength(),pBuf,nRet,NULL,NULL);
            SStringA strRet(pBuf,nRet);
            delete []pBuf;
            return strRet;
        }
    }
    return "";
}


}
