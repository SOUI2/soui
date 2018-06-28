#pragma once

namespace SOUI
{
    struct IRenderFactory;
    struct ZIP7RES_PARAM
    {
        enum {ZIPFILE,PEDATA} type;
        IRenderFactory *pRenderFac;
        union{
            LPCTSTR pszZipFile;
			
            struct{
                HINSTANCE hInst;
                LPCTSTR pszResName;
                LPCTSTR pszResType;
            }peInfo;
        };
        LPCSTR          pszPsw; 
		LPCTSTR			pszChildDir;
        void ZipFile(IRenderFactory *_pRenderFac,LPCTSTR _pszFile,LPCSTR _pszPsw =NULL, LPCTSTR _pszChildDir = NULL)
        {
            type=ZIPFILE;
            pszZipFile = _pszFile;
			pszChildDir = _pszChildDir;
            pRenderFac = _pRenderFac;
            pszPsw     = _pszPsw;
        }
        void ZipResource(IRenderFactory *_pRenderFac,HINSTANCE hInst,LPCTSTR pszResName,LPCTSTR pszResType=_T("zip"),LPCSTR _pszPsw =NULL, LPCTSTR _pszChildDir = NULL)
        {
            type=PEDATA;
            pRenderFac = _pRenderFac;
			pszChildDir = _pszChildDir;
            peInfo.hInst=hInst;
            peInfo.pszResName=pszResName;
            peInfo.pszResType=pszResType;
            pszPsw     = _pszPsw;
        }
    };
}